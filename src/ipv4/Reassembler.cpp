#include "Reassembler.hpp"
#include "Ipv4Header.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>
#include <utility>

std::optional<Packet>
Reassembler::add_fragment(const Ipv4Header &header, std::span<const std::byte> buffer) {
	const auto now = std::chrono::steady_clock::now();

	std::erase_if(
		pending_packets,
		[&](const auto &entry) {
			return now - entry.second.created_at >= std::chrono::seconds(30);
		});

	const ReassemblerKey key{
		.source = header.source,
		.destination = header.destination,
		.protocol = header.protocol,
		.id = header.id,
	};

	auto &pending = pending_packets[key];

	const std::size_t offset = header.fragment_offset * 8;
	const bool more_fragments = (header.flags & 0b001) != 0;

	// Copy payload into fragment

	if (buffer.empty() || buffer.size() > UINT16_MAX - offset) {
		pending_packets.erase(key);
		return std::nullopt;
	}

	// Every fragment except the final one must have a
	// payload length divisible by eight.
	if (more_fragments && buffer.size() % 8 != 0) {
		pending_packets.erase(key);
		return std::nullopt;
	}

	const std::size_t end = offset + buffer.size();

	for (const auto &existing : pending.fragments) {
		const std::size_t existing_end = existing.offset + existing.data.size();

		// Ignore duplicate
		if (existing.offset == offset &&
			existing.data.size() == buffer.size() &&
			std::equal(existing.data.begin(), existing.data.end(), buffer.begin())) {

			return std::nullopt;
		}

		const bool overlaps = offset < existing_end && existing.offset < end;

		// Reject overlap
		if (overlaps) {
			pending_packets.erase(key);
			return std::nullopt;
		}
	}

	if (offset == 0) {
		pending.first_header = header;
		pending.has_first_fragment = true;
	}

	if (!more_fragments) {
		if (pending.final_size && *pending.final_size != end) {
			pending_packets.erase(key);
			return std::nullopt;
		}
		pending.final_size = end;
	}

	pending.fragments.push_back(Fragment{
		.offset = offset,
		.data = std::vector<std::byte>{ buffer.begin(), buffer.end() },
	});

	// Sort by offset
	std::sort(
		pending.fragments.begin(),
		pending.fragments.end(),
		[](const Fragment &left, const Fragment &right) {
			return left.offset < right.offset;
		});

	if (!pending.has_first_fragment || !pending.final_size) {
		return std::nullopt;
	}

	std::size_t covered = 0;

	for (const auto &fragment : pending.fragments) {
		if (fragment.offset != covered) { // Comparison works because vector is sorted by offset
			return std::nullopt;		  // Gap found
		}

		covered += fragment.data.size();

		// We have more bytes than the reassmebled packet should have
		if (covered > *pending.final_size) {
			pending_packets.erase(key);
			return std::nullopt;
		}
	}

	if (covered != *pending.final_size) { // Gap
		return std::nullopt;
	}

	if (pending.first_header.header_length() + *pending.final_size > UINT16_MAX) { // Too big
		pending_packets.erase(key);
		return std::nullopt;
	}

	std::vector<std::byte> payload;
	payload.reserve(*pending.final_size);

	for (const auto &frag : pending.fragments) {
		payload.insert(payload.end(), frag.data.begin(), frag.data.end());
	}

	Ipv4Header completed_header = pending.first_header;

	completed_header.flags &= ~0b001;
	completed_header.fragment_offset = 0;
	completed_header.total_length = static_cast<std::uint16_t>(completed_header.header_length() + payload.size());

	completed_header.checksum = 0; // Just marks as unused/unimportant

	Packet completed{
		.header = completed_header,
		.payload = std::move(payload),
	};

	pending_packets.erase(key);

	return completed;
}
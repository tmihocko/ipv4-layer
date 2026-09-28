#include "Fragmenter.hpp"
#include "ipv4/Ipv4Header.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

std::vector<Packet> Fragmenter::fragment(const Ipv4Header &header, std::span<const std::byte> payload, std::size_t mtu) {
	if (header.header_length() >= mtu) {
		throw std::invalid_argument("MTU cannot hold IPv4 header");
	}

	const bool already_fragmented = header.fragment_offset != 0 || (header.flags & 0b001) != 0;

	if (already_fragmented) {
		throw std::invalid_argument("re-fragmentation is not supported yet");
	}

	const auto available = mtu - header.header_length();
	const auto fragment_capacity = (available / 8) * 8; // Round down to b/c offsets are expressed in units of eight bytes

	if (fragment_capacity == 0) {
		throw std::invalid_argument("MTU too small for fragmentation");
	}

	const std::size_t fragment_count = (payload.size() + fragment_capacity - 1) / fragment_capacity;

	std::vector<Packet> packets;

	packets.reserve(fragment_count);

	std::size_t byte_offset = 0;

	for (std::size_t i = 0; i < fragment_count; i++) {
		const std::size_t remaining = payload.size() - byte_offset;

		const std::size_t fragment_size = std::min(remaining, fragment_capacity);

		const bool has_more = byte_offset + fragment_size < payload.size();

		Ipv4Header fragment_header = header;

		fragment_header.fragment_offset = static_cast<std::uint16_t>(byte_offset / 8);
		fragment_header.total_length = static_cast<std::uint16_t>(header.header_length() + fragment_size);
		fragment_header.checksum = 0;

		if (has_more) {
			fragment_header.flags |= 0b001;
		} else {
			fragment_header.flags &= ~0b001;
		}

		std::vector<std::byte> fragment_payload(payload.begin() + byte_offset, payload.begin() + byte_offset + fragment_size);

		byte_offset += fragment_size;

		packets.push_back(Packet{
			.header = fragment_header,
			.payload = std::move(fragment_payload),
		});
	}

	return packets;
}

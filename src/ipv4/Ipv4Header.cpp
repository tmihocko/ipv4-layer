#include "Ipv4Header.hpp"
#include "util/BinaryReader.hpp"
#include <cstddef>
#include <cstdint>
#include <span>

std::uint16_t Ipv4Header::get_checksum(const std::byte *buffer, std::size_t n) {
	return get_checksum(std::span<const std::byte>{ buffer, n });
}

std::uint16_t Ipv4Header::get_checksum(std::span<const std::byte> header_bytes) {
	BinaryReader reader{ header_bytes };
	std::uint32_t sum = 0;

	while (reader.remaining() >= sizeof(std::uint16_t)) {
		sum += reader.read<std::uint16_t>();
	}

	// Fold carries back into the lower 16 bits.
	while ((sum >> 16) != 0) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return static_cast<std::uint16_t>(~sum);
}

std::optional<Ipv4Header> Ipv4Header::from_buffer(const std::byte *buffer, std::size_t n) {
	return from_buffer(std::span<const std::byte>{ buffer, n });
}

std::optional<Ipv4Header> Ipv4Header::from_buffer(std::span<const std::byte> buffer) {
	if (buffer.size() < minimum_wire_size) {
		return std::nullopt;
	}

	BinaryReader reader{ buffer };
	Ipv4Header header{};

	const auto first = reader.read<std::uint8_t>();

	header.version = (first & 0xF0) >> 4;
	header.ihl = first & 0x0F;

	if (header.version != 4 ||
		header.ihl < 5 ||
		buffer.size() < header.header_length()) {
		return std::nullopt;
	}

	const auto header_bytes =
		buffer.first(header.header_length());

	if (get_checksum(header_bytes) != 0) {
		return std::nullopt;
	}

	header.tos = reader.read<std::uint8_t>();
	header.total_length = reader.read<std::uint16_t>();
	header.id = reader.read<std::uint16_t>();

	const auto fragmentation = reader.read<std::uint16_t>();

	header.flags = static_cast<std::uint8_t>((fragmentation >> 13) & 0x07);
	header.fragment_offset = fragmentation & 0x1FFF;

	header.ttl = reader.read<std::uint8_t>();
	header.protocol = reader.read<std::uint8_t>();
	header.checksum = reader.read<std::uint16_t>();
	header.source = reader.read<std::uint32_t>();
	header.destination = reader.read<std::uint32_t>();

	if (header.total_length < header.header_length() || header.total_length > buffer.size()) {
		return std::nullopt;
	}

	return header;
}
#include "Ipv4Header.hpp"
#include "ipv4/Ipv4Address.hpp"
#include "util/BinaryReader.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include "util/Checksum.hpp"

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
		header.ihl != 5 ||
		buffer.size() < header.header_length()) {
		return std::nullopt;
	}

	const auto header_bytes =
		buffer.first(header.header_length());

	if (!Checksum::valid(header_bytes)) {
		return std::nullopt;
	}

	header.tos = reader.read<std::uint8_t>();
	header.total_length = reader.read<std::uint16_t>();
	header.id = reader.read<std::uint16_t>();

	const auto fragmentation = reader.read<std::uint16_t>();

	header.flags = static_cast<std::uint8_t>((fragmentation >> 13) & 0x07);
	header.fragment_offset = fragmentation & 0x1FFF;

	header.ttl = reader.read<std::uint8_t>();
	header.protocol = static_cast<Protocol>(reader.read<std::uint8_t>());
	header.checksum = reader.read<std::uint16_t>();
	header.source = IPv4Address{ reader.read<std::uint32_t>() };
	header.destination = IPv4Address{ reader.read<std::uint32_t>() };

	if (header.total_length < header.header_length() || header.total_length > buffer.size()) {
		return std::nullopt;
	}

	return header;
}
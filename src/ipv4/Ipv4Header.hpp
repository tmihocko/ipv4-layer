#ifndef IPV4_HEADER_HPP
#define IPV4_HEADER_HPP

#include "ipv4/Ipv4Address.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

/**
Not actually 20 bytes, separates smaller fields

Handles checksum on construction
*/
class Ipv4Header {
  public:
	static constexpr std::size_t minimum_wire_size = 20;

	std::uint8_t version;
	std::uint8_t ihl;
	std::uint8_t tos;
	std::uint16_t total_length;
	std::uint16_t id;
	std::uint8_t flags;
	std::uint16_t fragment_offset;
	std::uint8_t ttl;
	std::uint8_t protocol;
	std::uint16_t checksum;
	IPv4Address source;
	IPv4Address destination;

	[[nodiscard]] std::size_t header_length() const {
		return ihl * 4;
	}

	static std::uint16_t get_checksum(const std::byte *buffer, std::size_t n);
	static std::uint16_t get_checksum(std::span<const std::byte> header_bytes);

	static std::optional<Ipv4Header> from_buffer(const std::byte *buffer, std::size_t n);
	static std::optional<Ipv4Header> from_buffer(std::span<const std::byte> buffer);
};

struct Packet {
	Ipv4Header header;
	std::vector<std::byte> payload;
};

#endif
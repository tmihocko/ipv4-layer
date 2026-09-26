#ifndef IP_HEADER_HPP
#define IP_HEADER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

class IpHeader {
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
	std::uint32_t source;
	std::uint32_t destination;

	[[nodiscard]] std::size_t header_length() const {
		return ihl * 4;
	}

	static std::optional<IpHeader> from_buffer(std::span<const std::byte> buf);
};

#endif
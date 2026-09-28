#ifndef CHECKSUM_HPP
#define CHECKSUM_HPP

#include <cstddef>
#include <cstdint>
#include <span>

namespace Checksum {

inline std::uint16_t compute(
	std::span<const std::byte> bytes) {
	std::uint32_t sum = 0;
	std::size_t index = 0;

	while (index + 1 < bytes.size()) {
		const auto high =
			std::to_integer<std::uint8_t>(bytes[index]);

		const auto low =
			std::to_integer<std::uint8_t>(bytes[index + 1]);

		sum += static_cast<std::uint16_t>(
			(high << 8) | low);

		index += 2;
	}

	// An odd final byte is treated as the high byte
	// of a 16-bit word whose low byte is zero.
	if (index < bytes.size()) {
		sum += static_cast<std::uint16_t>(
			std::to_integer<std::uint8_t>(bytes[index]) << 8);
	}

	while ((sum >> 16) != 0) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return static_cast<std::uint16_t>(~sum);
}

inline bool valid(std::span<const std::byte> bytes) {
	return compute(bytes) == 0;
}

} // namespace Checksum

#endif
#ifndef ICMPHEADER_HPP
#define ICMPHEADER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

enum class IcmpType : std::uint8_t {
	EchoReply = 0,
	DestinationUnreachable = 3,
	SourceQuench = 4,
	Redirect = 5,
	EchoRequest = 8,
	TimeExceeded = 11,
	ParameterProblem = 12,
	TimestampRequest = 13,
	TimestampReply = 14,
};

class IcmpHeader {
  public:
	IcmpType type;
	std::uint8_t code;
	std::uint16_t checksum;
	std::uint32_t extended_header;

	static std::optional<IcmpHeader>
	from_buffer(std::span<const std::byte> buffer);
};

#endif // ICMPHEADER_HPP
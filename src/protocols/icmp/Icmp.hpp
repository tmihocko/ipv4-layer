#ifndef ICMP_HPP
#define ICMP_HPP

#include "ipv4/Ipv4Header.hpp"
#include "ipv4/Ipv4Output.hpp"
#include <cstddef>
#include <span>

namespace Icmp {

void handle(const Ipv4Header &header, std::span<const std::byte> payload, Ipv4Output &output);

void send_time_exceeded(
	const Ipv4Header &header,
	std::span<const std::byte> packet,
	Ipv4Output &output);

void send_destination_unreachable(
	const Ipv4Header &header,
	std::span<const std::byte> packet,
	std::uint8_t code,
	Ipv4Output &output);

void send_fragmentation_needed(
	const Ipv4Header &header,
	std::span<const std::byte> packet,
	std::uint16_t mtu,
	Ipv4Output &output);

} // namespace Icmp

#endif // ICMP_HPP
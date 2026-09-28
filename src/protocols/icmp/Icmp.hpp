#ifndef ICMP_HPP
#define ICMP_HPP

#include "ipv4/Ipv4Header.hpp"
#include "ipv4/Ipv4Output.hpp"
#include <cstddef>
#include <span>
#include <vector>

namespace Icmp {

void handle(const Ipv4Header &header, std::span<const std::byte> payload, Ipv4Output &output);

// type 0 code 0
std::vector<std::byte> make_reply(std::span<const std::byte> payload);

} // namespace Icmp

#endif // ICMP_HPP
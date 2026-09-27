#ifndef FRAGMENTER_HPP
#define FRAGMENTER_HPP

#include "ipv4/Ipv4Header.hpp"
#include <vector>

namespace Fragmenter {

std::vector<Packet> fragment(const Ipv4Header &header, std::span<const std::byte> payload, std::size_t mtu);

} // namespace Fragmenter

#endif // FRAGMENTER_HPP
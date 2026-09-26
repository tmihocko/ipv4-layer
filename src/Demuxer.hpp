#ifndef DEMUXER_HPP
#define DEMUXER_HPP

#include "Ipv4Header.hpp"
#include <cstddef>
#include <span>
namespace Demuxer {

void dispatch(const Ipv4Header &header, std::span<const std::byte> payload);

void handle_icmp(const Ipv4Header &header, std::span<const std::byte> payload);
void handle_tcp(const Ipv4Header &header, std::span<const std::byte> payload);
void handle_udp(const Ipv4Header &header, std::span<const std::byte> payload);

} // namespace Demuxer

#endif // DEMUXER_HPP
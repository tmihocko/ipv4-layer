#include "Ipv4Output.hpp"

Ipv4Output::Ipv4Output(Router &router, TunDevice &device, std::uint32_t local_address)
	: router_(router), device_(device), local_address_(local_address) {}

void Ipv4Output::send(std::uint32_t destination, std::uint8_t protocol, std::span<const std::byte> payload) {}

void Ipv4Output::forward(Ipv4Header header, std::span<const std::byte> payload) {}

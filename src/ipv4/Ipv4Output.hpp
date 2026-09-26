#ifndef IPV4OUTPUT_HPP
#define IPV4OUTPUT_HPP

#include "tun/TunDevice.hpp"
#include "ipv4/Ipv4Header.hpp"
#include "routing/Router.hpp"
#include <cstddef>
#include <span>

class Ipv4Output {
  public:
	Ipv4Output(Router &router, TunDevice &device, std::uint32_t local_address);

	void send(std::uint32_t destination, std::uint8_t protocol, std::span<const std::byte> payload);

	void forward(Ipv4Header header, std::span<const std::byte> payload);

  private:
	Router &router_;
	TunDevice &device_;
	std::uint32_t local_address_;
};
#endif // IPV4OUTPUT_HPP
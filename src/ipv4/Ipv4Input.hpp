#ifndef IPV4INPUT_HPP
#define IPV4INPUT_HPP

#include "ipv4/Reassembler.hpp"
#include "routing/Router.hpp"
#include "protocols/Demuxer.hpp"
#include "ipv4/Ipv4Output.hpp"

class Ipv4Input {
  public:
	Ipv4Input(Router &router, Reassembler &reassembler, Ipv4Output &output, std::uint32_t local_address);

	void process(std::span<const std::byte> packet);

  private:
	Router &router_;
	Reassembler &reassembler_;
	Ipv4Output &output_;

	std::uint32_t local_address_;
};

#endif // IPV4INPUT_HPP
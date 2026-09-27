#ifndef IPV4INPUT_HPP
#define IPV4INPUT_HPP

#include "ipv4/Ipv4Address.hpp"
#include "ipv4/Reassembler.hpp"
#include "ipv4/Ipv4Output.hpp"

class Ipv4Input {
  public:
	Ipv4Input(Reassembler &reassembler, Ipv4Output &output, IPv4Address local_address);

	void process(std::span<const std::byte> packet);

  private:
	Reassembler &reassembler_;
	Ipv4Output &output_;

	IPv4Address local_address_;
};

#endif // IPV4INPUT_HPP
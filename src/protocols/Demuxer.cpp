#include "Demuxer.hpp"
#include "ipv4/Ipv4Header.hpp"
#include "ipv4/Ipv4Output.hpp"
#include "protocols/icmp/Icmp.hpp"
#include <iostream>

void Demuxer::dispatch(const Ipv4Header &header, std::span<const std::byte> payload, std::span<const std::byte> packet, Ipv4Output &output) {
	switch (header.protocol) {
	case Protocol::ICMP:
		handle_icmp(header, payload, output);
		break;
	case Protocol::TCP:
		handle_tcp(header, payload);
		break;
	case Protocol::UDP:
		handle_udp(header, payload);
		break;
	default:
		Icmp::send_destination_unreachable(header, packet, 2, output);
		break;
	}
}

void Demuxer::handle_icmp(const Ipv4Header &header, std::span<const std::byte> payload, Ipv4Output &output) {
	Icmp::handle(header, payload, output);
}

void Demuxer::handle_tcp(const Ipv4Header &header, std::span<const std::byte> payload) {
	std::cout << "TCP not implemented." << std::endl;
}

void Demuxer::handle_udp(const Ipv4Header &header, std::span<const std::byte> payload) {
	std::cout << "UDP not implemented." << std::endl;
}

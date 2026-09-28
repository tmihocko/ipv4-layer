#include "Demuxer.hpp"
#include "ipv4/Ipv4Output.hpp"
#include "protocols/icmp/Icmp.hpp"
#include <iostream>

void Demuxer::dispatch(const Ipv4Header &header, std::span<const std::byte> payload, Ipv4Output &output) {
	switch (header.protocol) {
	case 1: // ICMP (ping requests)
		handle_icmp(header, payload, output);
		break;
	case 6: // TCP
		handle_tcp(header, payload);
		break;
	case 17: // UDP
		handle_udp(header, payload);
		break;
	default:
		// ICMP Destination Unreachable
		break;
	}
}

void Demuxer::handle_icmp(const Ipv4Header &header, std::span<const std::byte> payload, Ipv4Output &output) {
	Icmp::handle(header, payload, output);
}

void Demuxer::handle_tcp(const Ipv4Header &header, std::span<const std::byte> payload) {
	std::cout << "TCP implemented." << std::endl;
}

void Demuxer::handle_udp(const Ipv4Header &header, std::span<const std::byte> payload) {
	std::cout << "UDP implemented." << std::endl;
}

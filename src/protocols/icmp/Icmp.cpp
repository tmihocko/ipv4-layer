#include "Icmp.hpp"
#include "IcmpHeader.hpp"
#include "util/BinaryReader.hpp"
#include "util/Checksum.hpp"
#include <iostream>
#include <vector>

void Icmp::handle(const Ipv4Header &ip_header, std::span<const std::byte> payload, Ipv4Output &output) {

	if (payload.size() < 8 || !Checksum::valid(payload)) {
		return;
	}

	IcmpHeader icmp_header;
	BinaryReader reader{ payload };

	icmp_header.type = static_cast<IcmpType>(reader.read<std::uint8_t>());
	icmp_header.code = reader.read<std::uint8_t>();
	icmp_header.checksum = reader.read<std::uint16_t>();
	icmp_header.extended_header = reader.read<std::uint32_t>();

	switch (icmp_header.type) {

	case IcmpType::EchoRequest:
		if (icmp_header.code != 0) {
			return;
		} else {
			auto reply = Icmp::make_reply(payload);

			output.send(ip_header.source, 1, reply);
		}
		break;
	case IcmpType::EchoReply:
		if (icmp_header.code != 0) {
			return;
		} else {
			// Notify ping client
			std::cout
				<< "[ICMP]: received ICMP Echo Reply from "
				<< ip_header.source.to_string()
				<< std::endl;
		}
		break;

	case IcmpType::DestinationUnreachable:
		std::cout
			<< "[ICMP]: received ICMP Destination Unreachable from "
			<< ip_header.source.to_string()
			<< '\n';
		break;

	case IcmpType::TimeExceeded:
		std::cout
			<< "[ICMP]: received ICMP Time Exceeded from "
			<< ip_header.source.to_string()
			<< '\n';
		break;

	default:
		std::cout
			<< "[ICMP]: Message type not supported."
			<< std::endl;

		break;
	}
}

std::vector<std::byte> Icmp::make_reply(std::span<const std::byte> payload) {
	std::vector<std::byte> reply{ payload.begin(), payload.end() };

	reply[0] = static_cast<std::byte>(IcmpType::EchoReply); // Set type
	reply[1] = std::byte{ 0 };								// Code = 0

	// Reset checksum bytes
	reply[2] = std::byte{ 0 };
	reply[3] = std::byte{ 0 };

	const auto checksum = Checksum::compute(reply);

	reply[2] = static_cast<std::byte>((checksum & 0xFF00) >> 8);
	reply[3] = static_cast<std::byte>((checksum & 0x00FF) >> 0);

	return reply;
}

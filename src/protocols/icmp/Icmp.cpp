#include "Icmp.hpp"
#include "IcmpHeader.hpp"
#include "ipv4/Ipv4Header.hpp"
#include "util/BinaryReader.hpp"
#include "util/BinaryWriter.hpp"
#include "util/Checksum.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

static std::vector<std::byte> make_reply(std::span<const std::byte> payload) {
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

static std::vector<std::byte> make_error(IcmpType type, std::uint8_t code, std::uint32_t ext_header, std::span<const std::byte> old_packet) {
	BinaryWriter writer;

	writer.write<std::uint8_t, std::uint8_t, std::uint16_t, std::uint32_t>(
		static_cast<std::uint8_t>(type), code, 0, ext_header);

	const auto quote_size = std::min(old_packet.size(), Ipv4Header::minimum_wire_size + 8);

	writer.write_bytes(old_packet.first(quote_size));

	auto message = writer.move_data();
	const auto checksum = Checksum::compute(message);

	message[2] = static_cast<std::byte>((checksum & 0xFF00) >> 8);
	message[3] = static_cast<std::byte>((checksum & 0x00FF) >> 0);

	return message;
}

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
			auto reply = make_reply(payload);

			output.send(ip_header.source, Protocol::ICMP, reply);
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

void Icmp::send_time_exceeded(const Ipv4Header &header, std::span<const std::byte> old_packet, Ipv4Output &output) {

	const auto message = make_error(IcmpType::TimeExceeded, 0, 0, old_packet);

	output.send(header.source, Protocol::ICMP, message);
}

void Icmp::send_destination_unreachable(const Ipv4Header &header, std::span<const std::byte> packet, std::uint8_t code, Ipv4Output &output) {
	const auto message = make_error(IcmpType::DestinationUnreachable, code, 0, packet);

	output.send(header.source, Protocol::ICMP, message);
}

void Icmp::send_fragmentation_needed(const Ipv4Header &header, std::span<const std::byte> packet, std::uint16_t mtu, Ipv4Output &output) {
	const auto message = make_error(IcmpType::DestinationUnreachable, 4, static_cast<std::uint32_t>(mtu), packet);

	output.send(header.source, Protocol::ICMP, message);
}

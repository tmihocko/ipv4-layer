#include "Ipv4Output.hpp"
#include "ipv4/Fragmenter.hpp"
#include "ipv4/Ipv4Header.hpp"
#include "util/BinaryWriter.hpp"
#include "util/Checksum.hpp"
#include <cstdint>
#include "protocols/icmp/Icmp.hpp"

Ipv4Output::Ipv4Output(Router &router, TunDevice &tun, IPv4Address local_address)
	: router_(router), tun_(tun), local_address_(local_address) {}

void Ipv4Output::send(IPv4Address destination, Protocol protocol, std::span<const std::byte> payload) {
	if (payload.size() > UINT16_MAX - Ipv4Header::minimum_wire_size) {
		return;
	}

	Ipv4Header header{
		.version = 4,
		.ihl = 5,
		.tos = 0,
		.total_length = static_cast<std::uint16_t>(Ipv4Header::minimum_wire_size + payload.size()),
		.id = next_id_++,
		.flags = 0,
		.fragment_offset = 0,
		.ttl = 64,
		.protocol = protocol,
		.checksum = 0,
		.source = local_address_,
		.destination = destination,
	};

	transmit(header, payload);
}

void Ipv4Output::forward(Ipv4Header header, std::span<const std::byte> payload) {
	if (header.ttl <= 1) {
		Icmp::send_time_exceeded(header, payload, *this);
		return;
	}

	header.ttl--;

	transmit(header, payload);
}

void Ipv4Output::transmit(Ipv4Header header, std::span<const std::byte> payload) {
	const auto route = router_.lookup(header.destination);

	if (!route) {
		Icmp::send_destination_unreachable(header, payload, 0, *this);
		return;
	}

	if (header.total_length > route->mtu) {

		const bool dont_fragment = (header.flags & 0b010) != 0;
		if (dont_fragment) {
			Icmp::send_fragmentation_needed(header, payload, route->mtu, *this);
			return;
		} else {
			const auto fragments = Fragmenter::fragment(header, payload, route->mtu);

			for (const auto &fragment : fragments) {
				transmit(fragment.header, fragment.payload);
			}

			return;
		}
	}

	header.checksum = 0;
	BinaryWriter writer;

	writer.write<std::uint8_t>(
		((header.version & 0x0F) << 4) |
		(header.ihl & 0x0F));

	writer.write<std::uint8_t, std::uint16_t, std::uint16_t>(
		header.tos, header.total_length, header.id);

	writer.write<std::uint16_t>(
		static_cast<std::uint16_t>((header.flags & 0x07) << 13) |
		(header.fragment_offset & 0x1FFF));

	writer.write<std::uint8_t, std::uint8_t, std::uint16_t, std::uint32_t, std::uint32_t>(
		header.ttl, static_cast<std::uint8_t>(header.protocol), header.checksum, header.source.raw(), header.destination.raw());

	auto packet = writer.move_data();

	const auto checksum = Checksum::compute(packet);

	packet[10] = static_cast<std::byte>((checksum & 0xFF00) >> 8);
	packet[11] = static_cast<std::byte>((checksum & 0x00FF));

	packet.insert(packet.end(), payload.begin(), payload.end());

	tun_.write(packet.data(), packet.size());
}
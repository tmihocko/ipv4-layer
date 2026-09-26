
#include <iostream>
#include "Demuxer.hpp"
#include "TunDevice.hpp"
#include "Ipv4Header.hpp"

int main() {
	std::cout << "hi" << std::endl;

	constexpr std::uint32_t local_address = 0x0A000002; // 10.0.0.2

	TunDevice tun{ "tun0" };
	std::byte buf[2000];

	while (true) {
		auto n = tun.read(buf, sizeof(buf));

		if (n == 0) continue;

		const std::span<const std::byte> packet{ buf, n };

		const auto header = Ipv4Header::from_buffer(packet);
		const auto payload = packet.subspan(header->header_length(), header->total_length - header->header_length());

		if (!header) continue; // Invalid packet

		const bool reserved_bit = (header->flags & 0b100) != 0;
		if (reserved_bit) {
			continue; // Reserved bit should always be 0
		}

		if (header->destination == local_address) {
			// do fragment/Demuxer stuff
			const bool more_fragments = (header->flags & 0b001) != 0;
			const bool is_fragment = more_fragments || header->fragment_offset != 0;

			if (is_fragment) {
				// Send to fragment reassembly
			} else {
				if (header->ttl <= 1) { // Would hit 0 after next hop, time exceeded
					continue;
				}

				Demuxer::dispatch(*header, payload);
			}

		} else {
			// not for me, route it elsewhere
		}
	}

	return 0;
}

#include <iostream>
#include "Demuxer.hpp"
#include "TunDevice.hpp"
#include "Ipv4Header.hpp"
#include "Router.hpp"
#include "Reassembler.hpp"

int main() {
	std::cout << "hi" << std::endl;

	constexpr std::uint32_t local_address = 0x0A000002; // 10.0.0.2

	TunDevice tun{ "tun0" };
	Router router;
	Reassembler reassembler;

	std::byte buf[2000];

	while (true) {
		auto n = tun.read(buf, sizeof(buf));

		if (n == 0) continue;

		const std::span<const std::byte> packet{ buf, n };

		const auto header = Ipv4Header::from_buffer(packet);
		if (!header) continue; // Invalid packet

		const bool reserved_bit = (header->flags & 0b100) != 0;
		if (reserved_bit) continue; // Reserved bit should always be 0

		const auto payload = packet.subspan(header->header_length(), header->total_length - header->header_length());

		if (header->destination == local_address) {
			// do fragment/Demuxer stuff
			const bool more_fragments = (header->flags & 0b001) != 0;
			const bool is_fragment = more_fragments || header->fragment_offset != 0;

			if (is_fragment) {
				const auto completed = reassembler.add_fragment(*header, payload);

				if (!completed) continue;

				Demuxer::dispatch(completed->header, completed->payload);
			} else {
				Demuxer::dispatch(*header, payload);
			}

		} else {
			const auto route = router.lookup(header->destination);

			if (!route) continue;

			// Write to route
		}
	}

	return 0;
}

#include <iostream>
#include "ipv4/Ipv4Input.hpp"
#include "ipv4/Ipv4Output.hpp"
#include "tun/TunDevice.hpp"
#include "routing/Router.hpp"
#include "ipv4/Reassembler.hpp"

int main() {
	std::cout << "hi" << std::endl;

	constexpr std::uint32_t local_address = 0x0A000002; // 10.0.0.2

	TunDevice tun{ "tun0" };
	Router router;
	Reassembler reassembler;
	Ipv4Output output{ router, tun, local_address };
	Ipv4Input input{ reassembler, output, local_address };

	router.add_route({
		.network = 0,
		.prefix_length = 0,
		.gateway = 0x0A000001, // 10.0.0.1
		.mtu = 1500,
	});

	std::byte buf[2000];

	while (true) {
		auto n = tun.read(buf, sizeof(buf));

		if (n == 0) continue;

		const std::span<const std::byte> packet{ buf, n };

		input.process(packet);
	}

	return 0;
}
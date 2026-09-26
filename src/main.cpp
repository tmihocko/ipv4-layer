
#include <iostream>
#include "TunDevice.hpp"
#include "IpHeader.hpp"
#include <span>

int main() {
	std::cout << "hi" << std::endl;

	TunDevice tun{ "tun0" };
	std::byte buf[2000];

	while (true) {
		auto n = tun.read(buf, sizeof(buf));

		if (n == 0) continue;

		auto header = IpHeader::from_buf(std::span<const std::byte>{ buf, static_cast<std::size_t>(n) });

		if (!header) continue; // Invalid packet
	}

	return 0;
}
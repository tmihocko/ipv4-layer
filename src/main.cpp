
#include <iostream>
#include "TunDevice.hpp"
#include "Ipv4Header.hpp"

int main() {
	std::cout << "hi" << std::endl;

	TunDevice tun{ "tun0" };
	std::byte buf[2000];

	while (true) {
		auto n = tun.read(buf, sizeof(buf));

		if (n == 0) continue;

		auto header = Ipv4Header::from_buffer(buf, n);

		if (!header) continue; // Invalid packet
	}

	return 0;
}
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

std::string shellQuote(const std::string &value) {
	std::string result = "'";

	for (const char character : value) {
		if (character == '\'') {
			result += "'\\''";
		} else {
			result += character;
		}
	}

	return result + "'";
}

} // namespace

int main() {
	std::cout << "Building Docker image...\n";

	const std::string build_command = "docker build -t ipv4-stack " + shellQuote(IPV4STACK_SOURCE_DIR);

	const int build_result = std::system(build_command.c_str());

	if (build_result != 0) {
		std::cerr << "Failed to build Docker image.\n";
		return 1;
	}

	std::cout << "Starting IPv4 stack container...\n";

	const int run_result =
		std::system(
			"docker run "
			"--init "
			"--rm "
			"--cap-add=NET_ADMIN "
			"--device=/dev/net/tun "
			"ipv4-stack");

	if (run_result != 0) {
		std::cerr << "Docker container exited with an error.\n";
		return 1;
	}

	return 0;
}
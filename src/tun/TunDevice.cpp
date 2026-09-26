#include "TunDevice.hpp"
#include <cstddef>
#include <stdexcept>
#include <system_error>

std::size_t TunDevice::read(void *buf, std::size_t size) {
	while (true) {
		const ssize_t result = ::read(fd_, buf, size);

		if (result >= 0) {
			return static_cast<std::size_t>(result);
		}

		if (errno == EINTR) {
			continue;
		}

		throw std::system_error(
			errno,
			std::generic_category(),
			"failed to read TUN device");
	}
}

std::size_t TunDevice::write(const void *buf, std::size_t size) {
	while (true) {
		const ssize_t result = ::write(fd_, buf, size);

		if (result < 0) {
			if (errno == EINTR) {
				continue;
			}

			throw std::system_error(
				errno,
				std::generic_category(),
				"failed to write TUN device");
		}

		const auto written = static_cast<std::size_t>(result);

		if (written != size) {
			throw std::runtime_error("short write to TUN device");
		}

		return written;
	}
}

TunDevice::TunDevice(const char *device_name) {
	fd_ = open("/dev/net/tun", O_RDWR | O_CLOEXEC);

	if (fd_ < 0) {
		throw std::system_error(
			errno,
			std::generic_category(),
			"failed to open /dev/net/tun");
	}

	ifreq request{};
	request.ifr_flags = IFF_TUN | IFF_NO_PI;

	if (device_name != nullptr && device_name[0] != '\0') {
		std::strncpy(request.ifr_name, device_name, IFNAMSIZ - 1);
	}

	if (ioctl(fd_, TUNSETIFF, &request) < 0) {
		const int saved_errno = errno;
		close(fd_);
		fd_ = -1;

		throw std::system_error(
			saved_errno,
			std::generic_category(),
			"failed to configure TUN interface");
	}
}

TunDevice::~TunDevice() {
	if (fd_ >= 0) {
		close(fd_);
	}
}

int TunDevice::fd() const {
	return fd_;
}

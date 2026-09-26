/**

RAII wrapper for linux TUN

*/
#ifndef TUN_HPP
#define TUN_HPP

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <unistd.h>

class TunDevice {
  public:
	explicit TunDevice(const char *device_name);

	std::size_t read(void *buf, std::size_t count);

	std::size_t write(const void *buf, std::size_t count);

	[[nodiscard]] int fd() const;

	~TunDevice();

	TunDevice(const TunDevice &) = delete;
	TunDevice &operator=(const TunDevice &) = delete;
	TunDevice(TunDevice &&) = delete;
	TunDevice &operator=(TunDevice &&) = delete;

  private:
	int fd_ = -1;
};

#endif // TUN_HPP
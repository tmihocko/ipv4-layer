#ifndef IPV4_ADDRESS_HPP
#define IPV4_ADDRESS_HPP

#include <arpa/inet.h>
#include <compare>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

class IPv4Address {
  public:
	constexpr IPv4Address() = default;

	explicit constexpr IPv4Address(std::uint32_t raw)
		: raw_(raw) {}

	constexpr IPv4Address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d)
		: raw_{
			  (static_cast<std::uint32_t>(a) << 24) |
			  (static_cast<std::uint32_t>(b) << 16) |
			  (static_cast<std::uint32_t>(c) << 8) |
			  static_cast<std::uint32_t>(d)
		  } {}

	explicit IPv4Address(std::string_view text) {
		in_addr address{};
		std::string terminated{ text };

		if (inet_pton(AF_INET, terminated.c_str(), &address) != 1) {
			throw std::invalid_argument("invalid IPv4 address");
		}

		raw_ = ntohl(address.s_addr);
	}

	[[nodiscard]] constexpr std::uint32_t raw() const {
		return raw_;
	}

	auto operator<=>(const IPv4Address &) const = default;

  private:
	std::uint32_t raw_ = 0;
};

#endif // IPV4ADDRESS_HPP
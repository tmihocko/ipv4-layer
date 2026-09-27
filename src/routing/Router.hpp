#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

using IPv4Address = std::uint32_t;

struct Route {
	IPv4Address network;
	std::uint8_t prefix_length; // 0-32
	IPv4Address gateway;		// zero means directly connected

	std::size_t mtu;
};

class Router {
  public:
	void add_route(Route route);

	std::optional<Route> lookup(IPv4Address destination);

  private:
	std::vector<Route> routes_;
};

#endif // ROUTER_HPP
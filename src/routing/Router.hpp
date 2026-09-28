#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include "ipv4/Ipv4Address.hpp"
#include <vector>

struct Route {
	IPv4Address network;
	std::uint8_t prefix_length; // 0-32

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
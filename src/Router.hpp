#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

using IPv4Address = uint32_t;

struct Route {
	IPv4Address network;
	IPv4Address netmask;

	// 0 can mean "directly connected"
	IPv4Address gateway;
};

class Router {
  public:
	void add_route(const Route &route);

	std::optional<Route> lookup(IPv4Address destination);

  private:
	std::vector<Route> routes_;
};

#endif // ROUTER_HPP
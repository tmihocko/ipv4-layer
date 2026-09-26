#include "Router.hpp"
#include <optional>

void Router::add_route(const Route &route) {
	routes_.push_back(route);
}

std::optional<Route> Router::lookup(IPv4Address destination) {
	return std::nullopt;
}
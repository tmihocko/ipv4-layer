#include "Router.hpp"
#include <cstdint>
#include <optional>
#include <stdexcept>

static std::uint32_t prefix_mask(std::uint8_t length) {
	if (length == 0) {
		return 0;
	}

	return 0xFFFFFFFFu << (32 - length);
}

void Router::add_route(Route route) {
	if (route.prefix_length > 32) {
		throw std::invalid_argument("invalid IPv4 prefix length");
	}

	const auto mask = prefix_mask(route.prefix_length);
	route.network &= mask;

	routes_.push_back(route);
}

std::optional<Route> Router::lookup(IPv4Address destination) {
	std::optional<Route> best;

	for (const auto &route : routes_) {
		const auto mask = prefix_mask(route.prefix_length);

		if ((destination & mask) != route.network) {
			continue;
		}

		if (!best || route.prefix_length > best->prefix_length) {
			best = route;
		}
	}

	return best;
}
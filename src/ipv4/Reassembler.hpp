#ifndef REASSEMBLER_HPP
#define REASSEMBLER_HPP

#include "Ipv4Header.hpp"
#include <cstddef>
#include <map>
#include <optional>
#include <vector>

class Reassembler {
  public:
	// Returns packet when all fragments are present
	std::optional<Packet> add_fragment(const Ipv4Header &header, std::span<const std::byte> buffer);

  private:
	struct ReassemblerKey {
		IPv4Address source;
		IPv4Address destination;
		std::uint8_t protocol;
		std::uint16_t id;

		auto operator<=>(const ReassemblerKey &) const = default;
	};

	struct Fragment {
		std::size_t offset;
		std::vector<std::byte> data;
	};

	struct PendingPacket {
		Ipv4Header first_header;
		bool has_first_fragment = false;
		std::optional<std::size_t> final_size;
		std::vector<Fragment> fragments;
	};

	// (src_ip, dst_ip, protocol, identification) : header, payload
	std::map<ReassemblerKey, PendingPacket> pending_packets;
};

#endif // REASSEMBLER_HPP
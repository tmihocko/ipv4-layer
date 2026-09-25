#ifndef HEADER_HPP
#define HEADER_HPP

#include "PackedStruct.hpp"
#include <cstdint>
#include <linux/if_tun.h>

PACKED_STRUCT(IpHeader) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	std::uint8_t ihl : 4;	  // Header length
	std::uint8_t version : 4; // Version

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	std::uint8_t version : 4; // Version
	std::uint8_t ihl : 4;	  // Header length
#endif

	std::uint8_t tos; // Type of service
	std::uint16_t total_length;
	std::uint16_t id;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	std::uint16_t frag_offset : 13; // Fragment Offset (13 bits)
	std::uint16_t flags : 3;		// Flags (3 bits)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint16_t flags : 3;		   // Flags (3 bits)
	uint16_t frag_offset : 13; // Fragment Offset (13 bits)
#endif

	std::uint8_t ttl; // Time to live
	std::uint8_t protocol;
	std::uint16_t checksum;
	std::uint32_t src_ip;
	std::uint32_t dest_ip;
};

#endif // HEADER_HPP
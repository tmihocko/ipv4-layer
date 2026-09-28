#include "Ipv4Input.hpp"
#include "protocols/Demuxer.hpp"

Ipv4Input::Ipv4Input(Reassembler &reassembler, Ipv4Output &output, IPv4Address local_address)
	: reassembler_(reassembler), output_(output), local_address_(local_address) {}

void Ipv4Input::process(std::span<const std::byte> packet) {
	const auto header = Ipv4Header::from_buffer(packet);
	if (!header) return; // Invalid packet

	const bool reserved_bit = (header->flags & 0b100) != 0;
	if (reserved_bit) return; // Reserved bit should always be 0

	const auto payload = packet.subspan(header->header_length(), header->total_length - header->header_length());

	if (header->destination == local_address_) {
		// do fragment/Demuxer stuff
		const bool more_fragments = (header->flags & 0b001) != 0;
		const bool is_fragment = more_fragments || header->fragment_offset != 0;

		if (is_fragment) {
			const auto completed = reassembler_.add_fragment(*header, payload);

			if (!completed) return;

			Demuxer::dispatch(completed->header, completed->payload, output_);
		} else {
			Demuxer::dispatch(*header, payload, output_);
		}

	} else {
		output_.forward(*header, payload);
	}
}

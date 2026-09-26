#include "BinaryReader.hpp"
#include <cstddef>
#include <span>
#include <stdexcept>

std::vector<std::byte> BinaryReader::read_bytes(std::size_t n) {
	if (offset_ > data_.size() || n > data_.size() - offset_) {
		throw std::out_of_range("PacketReader::read_bytes: buffer underrun");
	}
	std::vector<std::byte> out(data_.begin() + offset_, data_.begin() + offset_ + n);
	offset_ += n;
	return out;
}

std::vector<std::byte> BinaryReader::read_remaining() {
	return read_bytes(remaining());
};

std::size_t BinaryReader::remaining() const noexcept {
	return data_.size() - offset_;
}

bool BinaryReader::at_end() const noexcept {
	return offset_ == data_.size();
}

void BinaryReader::assert_at_end() const {
	if (!at_end()) throw std::runtime_error("Data not at end");
}
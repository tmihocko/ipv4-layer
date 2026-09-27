#include "BinaryWriter.hpp"
#include <cstddef>
#include <span>
#include <stdexcept>

std::span<const std::byte> BinaryWriter::data() {
	assert_valid();
	return buffer_;
}

std::vector<std::byte> BinaryWriter::move_data() {
	assert_valid();
	valid_ = false;
	return std::move(buffer_);
}

std::uint32_t BinaryWriter::length() const {
	assert_valid();
	return static_cast<std::uint32_t>(buffer_.size());
}

void BinaryWriter::assert_valid() const {
	if (!valid_) throw std::logic_error("Writer has been consumed.");
}

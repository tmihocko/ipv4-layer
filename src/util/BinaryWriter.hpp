/**

Taken from another project I did

*/
#ifndef BINARY_WRITER_HPP
#define BINARY_WRITER_HPP
#include <chrono>
#include <span>
#include <string>
#include <type_traits>
#include <vector>
#include <cstring>

template <typename T>
concept BinarySerializable =
	std::is_integral_v<T> ||
	std::is_enum_v<T> ||
	std::is_same_v<T, std::string> ||
	std::is_same_v<T, std::chrono::system_clock::time_point>;

class BinaryWriter {
  public:
	BinaryWriter() = default;

	template <BinarySerializable... Ts>
		requires(sizeof...(Ts) > 1)
	BinaryWriter &write(const Ts &...values) {
		(write(values), ...);
		return *this;
	}

	template <BinarySerializable T>
	BinaryWriter &write(const T &value) {
		if constexpr (std::is_same_v<std::string, T>) {
			return write_string(value);
		} else if constexpr (std::is_same_v<bool, T>) {
			return write<std::uint8_t>(value ? 1 : 0);
		} else if constexpr (std::is_same_v<std::chrono::system_clock::time_point, T>) {
			const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();

			return write<std::int64_t>(static_cast<std::int64_t>(milliseconds));
		} else if constexpr (std::is_enum_v<T>) {
			using Underlying = std::underlying_type_t<T>;

			return write<Underlying>(static_cast<Underlying>(value));
		} else if constexpr (std::is_integral_v<T>) {
			return write_integral(value);
		} else {
			assert_valid();

			auto old = buffer_.size();
			buffer_.resize(old + sizeof(T));
			std::memcpy(buffer_.data() + old, &value, sizeof(T));

			return *this;
		}
	}

	template <typename T>
		requires std::is_integral_v<T>
	BinaryWriter &write_integral(T value) {
		assert_valid();

		using Unsigned = std::make_unsigned_t<T>;

		Unsigned bits;

		if constexpr (std::is_signed_v<T>) {
			bits = std::bit_cast<Unsigned>(value);
		} else {
			bits = static_cast<Unsigned>(value);
		}

		for (std::size_t index = sizeof(T); index > 0; index--) {
			const std::size_t shift = (index - 1) * 8;

			buffer_.push_back(static_cast<std::byte>((bits >> shift) & static_cast<Unsigned>(0xff)));
		}

		return *this;
	}

	// Writes until null terminator
	// Includes null terminator in message, maybe remove that later, will see
	BinaryWriter &write_string(const std::string &value) {
		assert_valid();
		const std::size_t byte_count = value.size() + 1;
		const std::size_t old = buffer_.size();

		buffer_.resize(old + byte_count);
		std::memcpy(buffer_.data() + old, value.c_str(), byte_count);

		return *this;
	}

	BinaryWriter &write_bytes(std::span<const std::byte> bytes) {
		assert_valid();

		buffer_.insert(buffer_.end(), bytes.begin(), bytes.end());

		return *this;
	}

	[[nodiscard]] std::vector<std::byte> move_data();
	std::span<const std::byte> data();
	std::uint32_t length() const;

  private:
	void assert_valid() const;

	std::vector<std::byte> buffer_;
	bool valid_ = true;
};

#endif
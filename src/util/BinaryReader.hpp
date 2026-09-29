/**

Taken from another project I did

 */
#ifndef BINARY_READER_HPP
#define BINARY_READER_HPP
#include <chrono>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <cstring>

#ifndef BINSER
#define BINSER

template <typename T>
concept BinarySerializable =
	std::is_integral_v<T> ||
	std::is_enum_v<T> ||
	std::is_same_v<T, std::string> ||
	std::is_same_v<T, std::chrono::system_clock::time_point>;

#endif

class BinaryReader {
  public:
	BinaryReader(std::span<const std::byte> bytes) : data_(bytes) {}

	template <BinarySerializable T>
	T read() {
		if constexpr (std::is_same_v<std::string, T>) {
			return read_string();
		} else if constexpr (std::is_same_v<bool, T>) {
			return read<std::uint8_t>() == 1;
		} else if constexpr (std::is_same_v<std::chrono::system_clock::time_point, T>) {
			using namespace std::chrono;
			const auto ms = read<std::int64_t>();

			return system_clock::time_point{ duration_cast<system_clock::time_point::duration>(milliseconds{ ms }) };
		} else if constexpr (std::is_enum_v<T>) {
			using Underlying = std::underlying_type_t<T>;

			return static_cast<T>(read<Underlying>());
		} else if constexpr (std::is_integral_v<T>) {
			return read_integral<T>();
		} else {
			if (offset_ > data_.size() || sizeof(T) > data_.size() - offset_) {
				throw std::out_of_range("PacketReader buffer underrun");
			}

			T value;
			std::memcpy(&value, data_.data() + offset_, sizeof(T));
			offset_ += sizeof(T);
			return value;
		}
	};

	template <BinarySerializable... Ts>
		requires(sizeof...(Ts) > 1)
	std::tuple<Ts...> read() {
		return std::tuple<Ts...>{ read<Ts>()... };
	}

	template <typename T>
		requires std::is_integral_v<T>
	T read_integral() {
		if (offset_ > data_.size() || sizeof(T) > data_.size() - offset_) {
			throw std::out_of_range("BinaryReader buffer underrun");
		}

		using Unsigned = std::make_unsigned_t<T>;

		Unsigned bits = 0;

		for (std::size_t index = 0; index < sizeof(T); ++index) {
			bits = static_cast<Unsigned>(bits << 8);
			bits |= static_cast<Unsigned>(std::to_integer<std::uint8_t>(data_[offset_ + index]));
		}

		offset_ += sizeof(T);

		if constexpr (std::is_signed_v<T>) {
			return std::bit_cast<T>(bits);
		} else {
			return static_cast<T>(bits);
		}
	}

	std::string read_string() {
		const auto begin = data_.begin() + offset_;
		const auto null_terminator = std::find(begin, data_.end(), std::byte{ 0 });

		if (null_terminator == data_.end()) {
			throw std::runtime_error("Packet string has no null terminator");
		}

		const auto length = static_cast<std::size_t>(std::distance(begin, null_terminator));
		const char *chars = reinterpret_cast<const char *>(data_.data() + offset_);

		std::string value(chars, length);
		offset_ += length + 1;

		return value;
	}

	std::vector<std::byte> read_remaining();
	std::vector<std::byte> read_bytes(std::size_t n);

	std::size_t remaining() const noexcept;
	bool at_end() const noexcept;
	void assert_at_end() const;

  private:
	std::span<const std::byte> data_;
	std::size_t offset_ = 0;
};

#endif
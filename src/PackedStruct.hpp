#if defined(_MSC_VER)

#define PACKED_STRUCT(name)             \
	__pragma(pack(push, 1)) struct name \
	__pragma(pack(pop))

#elif defined(__GNUC__) || defined(__clang__)

#define PACKED_STRUCT(name) \
	struct __attribute__((packed)) name

#else

#warning "Compiler not supported. Structure padding may occur."
#define PACKED_STRUCT(name) struct name

#endif
cmake_minimum_required(VERSION 3.28) # Version available in Ubuntu 24.04 

project(Ipv4Stack LANGUAGES CXX)

if(NOT LINUX)
    message(FATAL_ERROR "Unsupported platform")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE IPV4STACK_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

add_executable(Ipv4Stack
    ${IPV4STACK_SOURCES}
)

target_include_directories(Ipv4Stack PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(Ipv4Stack PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
	# -O3
)

set_target_properties(Ipv4Stack PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
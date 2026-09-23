cmake_minimum_required(VERSION 4.3)

project(Ipv4Stack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
	# -O3
)
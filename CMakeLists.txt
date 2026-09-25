cmake_minimum_required(VERSION 3.28) # Version available in Ubuntu 24.04 

project(Ipv4Stack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    add_subdirectory(launcher)
elseif(LINUX)
    add_subdirectory(src)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
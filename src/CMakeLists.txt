file(GLOB_RECURSE IPV4STACK_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

add_executable(Ipv4Stack
    ${IPV4STACK_SOURCES}
)

target_include_directories(Ipv4Stack PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_options(Ipv4Stack PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
)

set_target_properties(Ipv4Stack PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
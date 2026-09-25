add_executable(Ipv4StackLauncher
    main.cpp
)

target_compile_options(Ipv4StackLauncher PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
)

target_compile_definitions(Ipv4StackLauncher PRIVATE
    IPV4STACK_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

set_target_properties(Ipv4StackLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
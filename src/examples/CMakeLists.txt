cmake_minimum_required(VERSION 3.17)
project(transport_stack C)

set(CMAKE_C_STANDARD 99)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(isDebug TRUE)
else()
    message("no debug")
    set(isDebug FALSE)
endif()


if (isDebug)
    message("Debug build detected")
    add_compile_definitions(CONFIGURATION_DEBUG TYRAN_CONFIGURATION_DEBUG)
    set(CONFIGURATION_DEBUG 1)
endif()


set(DEPS ../../deps/)


file(GLOB_RECURSE deps_src FOLLOW_SYMLINKS
        "${DEPS}*/*/src/lib/*.c"
        )

message("something ${example_src}")

add_executable(transport_stack_example
        main.c
        ${deps_src}
    "${DEPS}piot/monotonic-time-c/src/platform/posix/monotonic_time_posix.c"
)

if (WIN32)
target_link_libraries(transport_stack_example transport_stack)
else()
target_link_libraries(transport_stack_example PUBLIC transport_stack m)
endif(WIN32)

target_compile_options(transport_stack_example PRIVATE -Wall -Wextra -Wshadow -Wstrict-aliasing -ansi -pedantic -Wno-declaration-after-statement -Wno-extra-semi-stmt -Wno-undef -Wno-unused-variable -Wno-unused-parameter -Wno-padded -Werror=implicit-function-declaration -Werror=incompatible-pointer-types  -Werror=missing-prototypes -Werror=int-conversion -Werror=return-type -Werror=incompatible-function-pointer-types) # -Werror=incompatible-pointer-types-discards-qualifiers
if (COMPILER_CLANG)
    target_compile_options(transport_stack_example PRIVATE -Wmost -Weverything -Werror=missing-variable-declarations)
endif()

cmake_minimum_required(VERSION 3.17)
project(transport_stack C)

set(CMAKE_C_STANDARD 99)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(isDebug TRUE)
else()
  message("no debug")
  set(isDebug FALSE)
endif()

if(isDebug)
  message("Debug build detected")
  add_compile_definitions(CONFIGURATION_DEBUG TYRAN_CONFIGURATION_DEBUG)
  set(CONFIGURATION_DEBUG 1)
endif()

set(DEPS ../../deps/)

add_subdirectory("${DEPS}piot/udp-server-connections/src/lib/" temp)

file(GLOB_RECURSE deps_src FOLLOW_SYMLINKS "${DEPS}*/*/src/lib/*.c"
     "${DEPS}*/*/src/platform/posix/*.c")

file(GLOB src FOLLOW_SYMLINKS "*.c")

add_library(transport_stack STATIC ${src})

target_compile_options(
  transport_stack
  PRIVATE -Wall
          -Wextra
          -Wshadow
          -Wstrict-aliasing
          -ansi
          -pedantic
          -Wno-declaration-after-statement
          -Wno-extra-semi-stmt
          -Wno-undef
          -Wno-unused-variable
          -Wno-unused-parameter
          -Wno-padded
          -Werror=implicit-function-declaration
          -Werror=incompatible-pointer-types
          -Werror=missing-prototypes
          -Werror=int-conversion
          -Werror=return-type
          -Werror=incompatible-function-pointer-types)
if(COMPILER_CLANG)
  target_compile_options(
    transport_stack PRIVATE -Wmost -Weverything
                            -Werror=missing-variable-declarations)
endif()

target_include_directories(transport_stack PUBLIC ${DEPS}piot/clog/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/tiny-libc/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/imprint/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/flood-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/discoid-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/udp-server-c/src/include)
target_include_directories(
  transport_stack PUBLIC ${DEPS}piot/udp-server-connections/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/udp-client-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/hazy-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/monotonic-time-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/udp-transport/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/conclave-client-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/conclave-serialize-c/src/include)
target_include_directories(
  transport_stack PUBLIC ${DEPS}piot/udp-connections-serialize/src/include)
target_include_directories(
  transport_stack PUBLIC ${DEPS}piot/udp-connections-client-c/src/include)
target_include_directories(transport_stack
                           PUBLIC ${DEPS}piot/secure-random-c/src/include)
target_include_directories(transport_stack PUBLIC ../include)

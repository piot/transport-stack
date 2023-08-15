cmake_minimum_required(VERSION 3.17)
project(transport_stack C)

add_subdirectory(deps/piot/clog/src/lib)
add_subdirectory(deps/piot/tiny-libc/src/lib)
add_subdirectory(deps/piot/datagram-transport-c/src/lib)
add_subdirectory(deps/piot/discoid-c/src/lib)
add_subdirectory(deps/piot/flood-c/src/lib)
add_subdirectory(deps/piot/hazy-c/src/lib)
add_subdirectory(deps/piot/imprint/src/lib)
add_subdirectory(deps/piot/monotonic-time-c/src/lib)
add_subdirectory(deps/piot/secure-random-c/src/lib)
add_subdirectory(deps/piot/time-tick-c/src/lib)
add_subdirectory(deps/piot/udp-client-c/src/lib)
add_subdirectory(deps/piot/udp-connections-client-c/src/lib)
add_subdirectory(deps/piot/udp-connections-serialize/src/lib)
add_subdirectory(deps/piot/udp-server-c/src/lib)
add_subdirectory(deps/piot/udp-server-connections/src/lib)

# Relay Client
add_subdirectory(deps/piot/relay-serialize-c/src/lib)
add_subdirectory(deps/piot/relay-client-c/src/lib)


add_subdirectory("lib")
#add_subdirectory("test")
#add_subdirectory("examples")

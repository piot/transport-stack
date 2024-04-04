/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/transport-stack
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef TRANSPORT_STACK_MULTI_H
#define TRANSPORT_STACK_MULTI_H

#include <transport-stack/lower_level.h>
#include <transport-stack/types.h>
#include <udp-client/udp_client.h>
#include <udp-server-connections/connections.h>

struct ImprintAllocatorWithFree;
struct ImprintAllocator;

typedef struct TransportStackMulti {
    DatagramTransportMulti multiTransport;
    struct ImprintAllocatorWithFree* allocatorWithFree;
    struct ImprintAllocator* allocator;
    UdpServerSocket udpServer;
    DatagramTransport udpServerTransport;
    UdpServerConnections udpServerConnections;
    TransportStackMode mode;
    Clog log;
} TransportStackMulti;

int transportStackMultiInit(TransportStackMulti* self, struct ImprintAllocator* allocator,
                            struct ImprintAllocatorWithFree* allocatorWithFree, TransportStackMode mode, Clog log);
void transportStackMultiUpdate(TransportStackMulti* self);
int transportStackMultiListen(TransportStackMulti* self, const char* host, size_t port);

#endif

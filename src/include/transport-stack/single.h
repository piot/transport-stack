/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef TRANSPORT_STACK_SINGLE_H
#define TRANSPORT_STACK_SINGLE_H

#include <conclave-client/network_realizer.h>
#include <hazy/transport.h>
#include <transport-stack/conclave.h>
#include <udp-client/udp_client.h>
#include <udp-connections-client/client.h>
#include <udp-transport/multi_single.h>

struct ImprintAllocatorWithFree;
struct ImprintAllocator;



typedef struct TransportStackSingle {
    TransportStackConclave conclave;
    struct ImprintAllocatorWithFree* allocatorWithFree;
    struct ImprintAllocator* allocator;
    DatagramTransportSingleToFromMulti sendAndReceiveOnlyFromHost;
    UdpTransportInOut singleTransport;
    Clog log;
    TransportStackMode mode;
    UdpConnectionsClient connectionsClient;

} TransportStackSingle;

void transportStackSingleInit(TransportStackSingle* self, struct ImprintAllocator* allocator,
                              struct ImprintAllocatorWithFree* allocatorWithFree, TransportStackMode mode, Clog log);
int transportStackSingleConnect(TransportStackSingle* self, const char* host, size_t port);
bool transportStackSingleIsConnected(const TransportStackSingle* self);
void transportStackSingleUpdate(TransportStackSingle* self);
void transportStackSingleSetInternetSimulationMode(TransportStackSingle* self,
                                                   TransportStackInternetSimulationMode mode);

#endif

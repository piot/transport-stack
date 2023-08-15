/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef TRANSPORT_STACK_SINGLE_H
#define TRANSPORT_STACK_SINGLE_H

#include <datagram-transport/multi_single.h>
#include <hazy/transport.h>
#include <relay-client/client.h>
#include <time-tick/time_tick.h>
#include <transport-stack/lower_level.h>
#include <udp-client/udp_client.h>
#include <udp-connections-client/client.h>

struct ImprintAllocatorWithFree;
struct ImprintAllocator;

typedef struct TransportStackSingle {
    TransportStackLowerLevel lowerLevel;
    struct ImprintAllocatorWithFree* allocatorWithFree;
    struct ImprintAllocator* allocator;
    DatagramTransportSingleToFromMulti sendAndReceiveOnlyFromHost;
    DatagramTransport singleTransport;
    Clog log;
    TransportStackMode mode;
    UdpConnectionsClient connectionsClient;
    RelayClient relayClient;
    TimeTick timeTick;
    RelaySerializeUserSessionId userSessionIdForRelay;
} TransportStackSingle;

void transportStackSingleInit(TransportStackSingle* self, struct ImprintAllocator* allocator,
                              struct ImprintAllocatorWithFree* allocatorWithFree, TransportStackMode mode, Clog log);
int transportStackSingleConnect(TransportStackSingle* self, const char* host, size_t port);
bool transportStackSingleIsConnected(const TransportStackSingle* self);
void transportStackSingleUpdate(TransportStackSingle* self);
void transportStackSingleSetInternetSimulationMode(TransportStackSingle* self,
                                                   TransportStackInternetSimulationMode mode);

#endif

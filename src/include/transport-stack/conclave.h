/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef TRANSPORT_STACK_CONCLAVE_H
#define TRANSPORT_STACK_CONCLAVE_H

#include <conclave-client/network_realizer.h>
#include <hazy/transport.h>
#include <transport-stack/types.h>
#include <udp-client/udp_client.h>
#include <udp-transport/multi_single.h>

typedef enum TransportStackInternetSimulationMode {
    TransportStackInternetSimulationModeGood,
    TransportStackInternetSimulationModeRecommended,
    TransportStackInternetSimulationModeWorstCase
} TransportStackInternetSimulationMode;

typedef struct TransportStackConclave {
    ClvClientRealize conclaveClient;
    UdpClientSocket udpClient;
    UdpTransportInOut udpClientTransport;
    TransportStackMode mode;
    HazyDatagramTransportInOut hazyTransport;
    Clog log;
    TransportStackInternetSimulationMode internetSimulationMode;
} TransportStackConclave;

typedef struct TransportStackConclaveSetup {
    const char* username;
    struct ImprintAllocator* allocator;
    struct ImprintAllocatorWithFree* allocatorWithFree;
    Clog log;
    TransportStackMode mode;
} TransportStackConclaveSetup;

int transportStackConclaveInit(TransportStackConclave* self, TransportStackConclaveSetup setup);
int transportStackConclaveEstablish(TransportStackConclave* self, const char* host, size_t port);
void transportStackConclaveUpdate(TransportStackConclave* self);
void transportStackConclaveSetInternetSimulationMode(TransportStackConclave* self, TransportStackInternetSimulationMode mode);

#endif

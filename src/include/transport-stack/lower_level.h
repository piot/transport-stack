/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef TRANSPORT_STACK_LOWER_LEVEL_H
#define TRANSPORT_STACK_LOWER_LEVEL_H

#include <hazy/transport.h>
#include <transport-stack/types.h>
#include <udp-client/udp_client.h>
#include <datagram-transport/multi_single.h>


typedef enum TransportStackInternetSimulationMode {
    TransportStackInternetSimulationModeGood,
    TransportStackInternetSimulationModeRecommended,
    TransportStackInternetSimulationModeWorstCase
} TransportStackInternetSimulationMode;

typedef struct TransportStackLowerLevel {
    DatagramTransport transportToUse;
    UdpClientSocket udpClient;
    DatagramTransport udpClientTransport;
    TransportStackMode mode;
    HazyDatagramTransportInOut hazyTransport;
    Clog log;
    TransportStackInternetSimulationMode internetSimulationMode;
} TransportStackLowerLevel;

typedef struct TransportStackConclaveSetup {
    const char* username;
    struct ImprintAllocator* allocator;
    struct ImprintAllocatorWithFree* allocatorWithFree;
    Clog log;
    TransportStackMode mode;
} TransportStackConclaveSetup;

int transportStackLowerLevelInit(TransportStackLowerLevel* self, TransportStackConclaveSetup setup);
int transportStackLowerLevelEstablish(TransportStackLowerLevel* self, const char* host, size_t port);
void transportStackLowerLevelUpdate(TransportStackLowerLevel* self);
void transportStackLowerLevelSetInternetSimulationMode(TransportStackLowerLevel* self, TransportStackInternetSimulationMode mode);

#endif

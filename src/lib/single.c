/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/transport-stack
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <transport-stack/single.h>

static int transportStackSingleTick(void* _self)
{
    TransportStackSingle* self = (TransportStackSingle*) _self;

    switch (self->mode) {
        case TransportStackModeLocalUdp: {
            udpConnectionsClientUpdate(&self->connectionsClient);
        } break;
        case TransportStackModeRelay: {
            relayClientUpdate(&self->relayClient, self->timeTick.tickedUpToMonotonic);
        } break;
    }
    hazyDatagramTransportInOutUpdate(&self->lowerLevel.hazyTransport);

    return 0;
}

void transportStackSingleInit(TransportStackSingle* self, struct ImprintAllocator* allocator,
                              struct ImprintAllocatorWithFree* allocatorWithFree, TransportStackMode mode, Clog log)
{
    self->log = log;
    self->mode = mode;
    self->allocator = allocator;
    self->allocatorWithFree = allocatorWithFree;
    const size_t targetDeltaTimeMs = 16U;
    timeTickInit(&self->timeTick, targetDeltaTimeMs, self, transportStackSingleTick, monotonicTimeMsNow(), log);
}

int transportStackSingleConnect(TransportStackSingle* self, const char* host, size_t port)
{
    switch (self->mode) {
        case TransportStackModeLocalUdp: {
            CLOG_C_DEBUG(&self->log, "connecting to '%s' %zu. using udp connections client", host, port)
            // Reuse conclave for udp client part
            TransportStackConclaveSetup setup;
            setup.allocator = self->allocator;
            setup.allocatorWithFree = self->allocatorWithFree;
            setup.mode = TransportStackModeLocalUdp;
            setup.username = "";
            setup.log.config = self->log.config;
            setup.log.constantPrefix = "singleUdp";
            transportStackLowerLevelInit(&self->lowerLevel, setup);
            transportStackLowerLevelEstablish(&self->lowerLevel, host, port);
            CLOG_C_DEBUG(&self->log, "udp connections client is put on top of hazy internet emulator")
            int initErr = udpConnectionsClientInit(&self->connectionsClient, self->lowerLevel.hazyTransport.transport,
                                                   self->log);
            if (initErr < 0) {
                return initErr;
            }
            self->singleTransport = self->connectionsClient.transport;
        } break;
        case TransportStackModeRelay: {
            CLOG_C_DEBUG(&self->log, "connecting to '%s' %zu, using relay", host, port)
            relayClientInit(&self->relayClient, self->userSessionIdForRelay, self->lowerLevel.hazyTransport.transport,
                            self->allocator, "prefix", self->log);
        } break;
    }

    return 0;
}

bool transportStackSingleIsConnected(const TransportStackSingle* self)
{
    switch (self->mode) {
        case TransportStackModeLocalUdp:
            return self->connectionsClient.phase == UdpConnectionsClientPhaseConnected;
            case TransportStackModeRelay:
            return false;
    }
}

void transportStackSingleUpdate(TransportStackSingle* self)
{
    timeTickUpdate(&self->timeTick, monotonicTimeMsNow());
}

void transportStackSingleSetInternetSimulationMode(TransportStackSingle* self,
                                                   TransportStackInternetSimulationMode mode)
{
    transportStackLowerLevelSetInternetSimulationMode(&self->lowerLevel, mode);
}

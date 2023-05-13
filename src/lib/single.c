/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <transport-stack/single.h>
#include <transport-stack/types.h>

static int transportStackSingleTick(void* _self)
{
    TransportStackSingle* self = (TransportStackSingle*) _self;

    switch (self->mode) {
        case TransportStackModeLocalUdp: {
            udpConnectionsClientUpdate(&self->connectionsClient);
            hazyDatagramTransportInOutUpdate(&self->conclave.hazyTransport);
        } break;
    }

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
            CLOG_C_DEBUG(&self->log, "connecting to '%s' %d. reusing conclave for udp client connection", host, port)
            // Reuse conclave for udp client part
            TransportStackConclaveSetup setup;
            setup.allocator = self->allocator;
            setup.allocatorWithFree = self->allocatorWithFree;
            setup.mode = TransportStackModeLocalUdp;
            setup.username = "";
            setup.log.config = self->log.config;
            setup.log.constantPrefix = "singleUdp";
            transportStackConclaveInit(&self->conclave, setup);
            transportStackConclaveEstablish(&self->conclave, host, port);
            CLOG_C_DEBUG(&self->log, "udp connections client is put on top of hazy internet emulator");
            int initErr = udpConnectionsClientInit(&self->connectionsClient, self->conclave.hazyTransport.transport,
                                                   self->log);
            if (initErr < 0) {
                return initErr;
            }
            self->singleTransport = self->connectionsClient.transport;
        } break;
        case TransportStackModeConclave: {
            CLOG_C_DEBUG(&self->log, "connecting conclave to '%s' %d", host, port)
            datagramTransportSingleToFromMultiInit(&self->sendAndReceiveOnlyFromHost,
                                                   self->conclave.conclaveClient.client.multiTransport, 0);
        } break;
        default:
            CLOG_C_ERROR(&self->log, "unknown mode %d", self->mode)
            break;
    }

    return 0;
}

bool transportStackSingleIsConnected(const TransportStackSingle* self)
{
    switch (self->mode) {
        case TransportStackModeLocalUdp:
            return self->connectionsClient.phase == UdpConnectionsClientPhaseConnected;
            break;
        case TransportStackModeConclave:
            return false;
            break;
        default:
            CLOG_C_ERROR(&self->log, "unknown mode")
    }
}

void transportStackSingleUpdate(TransportStackSingle* self)
{
    timeTickUpdate(&self->timeTick, monotonicTimeMsNow());
}

void transportStackSingleSetInternetSimulationMode(TransportStackSingle* self,
                                                   TransportStackInternetSimulationMode mode)
{
    transportStackConclaveSetInternetSimulationMode(&self->conclave, mode);
}

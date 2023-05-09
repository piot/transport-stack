/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <transport-stack/single.h>
#include <transport-stack/types.h>

void transportStackSingleInit(TransportStackSingle* self, struct ImprintAllocator* allocator,
                              struct ImprintAllocatorWithFree* allocatorWithFree, TransportStackMode mode, Clog log)
{
    self->log = log;
    self->mode = mode;
    self->allocator = allocator;
    self->allocatorWithFree = allocatorWithFree;
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
            self->singleTransport = self->conclave.hazyTransport.transport;
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

void transportStackSingleUpdate(TransportStackSingle* self)
{
    switch (self->mode) {
        case TransportStackModeLocalUdp: {
            hazyDatagramTransportInOutUpdate(&self->conclave.hazyTransport);
        } break;
    }
}

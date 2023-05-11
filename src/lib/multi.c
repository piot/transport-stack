/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <transport-stack/multi.h>

void transportStackMultiUpdate(TransportStackMulti* self)
{
    switch (self->mode) {
        case TransportStackModeLocalUdp: {
        } break;
    }
}

int transportStackMultiInit(TransportStackMulti* self, TransportStackMode mode, Clog log)
{
    self->log = log;
    self->mode = mode;

    CLOG_C_DEBUG(&self->log, "initializing multi transport for mode %d", mode)

    return 0;
}

int transportStackMultiListen(TransportStackMulti* self, const char* host, size_t port)
{
    switch (self->mode) {
        case TransportStackModeConclave: {
            TransportStackConclaveSetup conclaveSetup;
            conclaveSetup.log = self->log;
            conclaveSetup.allocator = self->allocator;
            conclaveSetup.allocatorWithFree = self->allocatorWithFree;
            conclaveSetup.mode = TransportStackModeConclave;
            conclaveSetup.username = "Piot";
            transportStackConclaveInit(&self->conclave, conclaveSetup);
        } break;
        case TransportStackModeLocalUdp: {
            int errorCode = udpServerInit(&self->udpServer, port, false);
            if (errorCode < 0) {
                CLOG_C_ERROR(&self->log, "could not host on port %d", port)
                return errorCode;
            }
            CLOG_C_DEBUG(&self->log, "listening on UDP port %d", port);

            udpServerConnectionsInit(&self->udpServerConnections, &self->udpServer, self->log);
            CLOG_C_DEBUG(&self->log, "UDP multi connections server is initialized");

            self->multiTransport = self->udpServerConnections.multiTransport;
        }
    }

    return 0;
}

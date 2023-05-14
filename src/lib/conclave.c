/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <transport-stack/conclave.h>

static int udpClientTransportSend(void* _self, const uint8_t* data, size_t size)
{
    TransportStackConclave* self = _self;
    return udpClientSend(&self->udpClient, data, size);
}

static ssize_t udpClientSideReceive(void* _self, uint8_t* data, size_t size)
{
    TransportStackConclave* self = _self;
    return udpClientReceive(&self->udpClient, data, size);
}

static int addHazyAndConclaveOnTopOfTransport(TransportStackConclave* self, struct ImprintAllocator* allocator,
                                              struct ImprintAllocatorWithFree* allocatorWithFree)
{
    Clog hazyLog;
    hazyLog.config = &g_clog;
    hazyLog.constantPrefix = "Hazy";
    hazyDatagramTransportInOutInit(&self->hazyTransport, self->udpClientTransport, allocator, allocatorWithFree,
                                   hazyConfigGoodCondition(), hazyLog);

    CLOG_C_DEBUG(&self->log, "hazy emulator is put on top of udp client transport")

    self->conclaveClient.client.transport = self->hazyTransport.transport;

    return 0;
}

int transportStackConclaveInit(TransportStackConclave* self, TransportStackConclaveSetup conclaveSetup)
{
    self->log = conclaveSetup.log;
    datagramTransportInit(&self->udpClientTransport, self, udpClientTransportSend, udpClientSideReceive);

    addHazyAndConclaveOnTopOfTransport(self, conclaveSetup.allocator, conclaveSetup.allocatorWithFree);

    self->mode = conclaveSetup.mode;

    ClvClientRealizeSettings setup;
    setup.transport = self->hazyTransport.transport;
    setup.log = conclaveSetup.log;
    setup.memory = conclaveSetup.allocator;
    setup.username = conclaveSetup.username;
    clvClientRealizeInit(&self->conclaveClient, &setup);

    transportStackConclaveSetInternetSimulationMode(self, TransportStackInternetSimulationModeGood);

    return 0;
}

int transportStackConclaveEstablish(TransportStackConclave* self, const char* host, size_t port)
{
    CLOG_C_DEBUG(&self->log, "establish udp client to %s %d", host, port)
    int errorCode = udpClientInit(&self->udpClient, host, port);
    if (errorCode < 0) {
        CLOG_ERROR("could not start udp client")
        return errorCode;
    }
    return 0;
}

void transportStackConclaveUpdate(TransportStackConclave* self)
{
    if (self->mode == TransportStackModeConclave) {
        clvClientRealizeUpdate(&self->conclaveClient, monotonicTimeMsNow());
        hazyDatagramTransportInOutUpdate(&self->hazyTransport);
    }
}

void transportStackConclaveSetInternetSimulationMode(TransportStackConclave* self,
                                                     TransportStackInternetSimulationMode mode)
{
    HazyConfig config;
    switch (mode) {
        case TransportStackInternetSimulationModeGood:
            CLOG_C_DEBUG(&self->log, "internet simulation: good")
            config = hazyConfigGoodCondition();
            break;
        case TransportStackInternetSimulationModeRecommended:
            CLOG_C_DEBUG(&self->log, "internet simulation: recommended")
            config = hazyConfigRecommended();
            break;
        case TransportStackInternetSimulationModeWorstCase:
            CLOG_C_DEBUG(&self->log, "internet simulation: worst case")
            config = hazyConfigWorstCase();
            break;
    }
    self->internetSimulationMode = mode;

    hazySetConfig(&self->hazyTransport.hazy, config);
}

/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/transport-stack
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <transport-stack/lower_level.h>

static int udpClientTransportSend(void* _self, const uint8_t* data, size_t size)
{
    TransportStackLowerLevel* self = _self;
    return udpClientSend(&self->udpClient, data, size);
}

static ssize_t udpClientSideReceive(void* _self, uint8_t* data, size_t size)
{
    TransportStackLowerLevel* self = _self;
    return udpClientReceive(&self->udpClient, data, size);
}

static int addHazyAndConclaveOnTopOfTransport(TransportStackLowerLevel* self, struct ImprintAllocator* allocator,
                                              struct ImprintAllocatorWithFree* allocatorWithFree)
{
    Clog hazyLog;
    hazyLog.config = &g_clog;
    hazyLog.constantPrefix = "Hazy";
    hazyDatagramTransportInOutInit(&self->hazyTransport, self->udpClientTransport, allocator, allocatorWithFree,
                                   hazyConfigGoodCondition(), hazyLog);

    CLOG_C_DEBUG(&self->log, "hazy emulator is put on top of udp client transport")

    self->transportToUse = self->hazyTransport.transport;

    return 0;
}

int transportStackLowerLevelInit(TransportStackLowerLevel* self, TransportStackConclaveSetup conclaveSetup)
{
    self->log = conclaveSetup.log;
    datagramTransportInit(&self->udpClientTransport, self, udpClientTransportSend, udpClientSideReceive);

    addHazyAndConclaveOnTopOfTransport(self, conclaveSetup.allocator, conclaveSetup.allocatorWithFree);

    self->mode = conclaveSetup.mode;

    transportStackLowerLevelSetInternetSimulationMode(self, TransportStackInternetSimulationModeGood);

    return 0;
}

int transportStackLowerLevelEstablish(TransportStackLowerLevel* self, const char* host, size_t port)
{
    CLOG_C_DEBUG(&self->log, "establish udp client to %s %zu", host, port)
    int errorCode = udpClientInit(&self->udpClient, host, (uint16_t) port);
    if (errorCode < 0) {
        CLOG_ERROR("could not start udp client")
        //return errorCode;
    }
    return 0;
}

void transportStackLowerLevelUpdate(TransportStackLowerLevel* self)
{
    hazyDatagramTransportInOutUpdate(&self->hazyTransport);
}

void transportStackLowerLevelSetInternetSimulationMode(TransportStackLowerLevel* self,
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

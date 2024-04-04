/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/transport-stack
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/console.h>
#include <imprint/default_setup.h>
#include <stdio.h>
#include <transport-stack/multi.h>
#include <transport-stack/single.h>
#include <unistd.h>

clog_config g_clog;

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    g_clog.log = clog_console;
    g_clog.level = CLOG_TYPE_DEBUG;

    CLOG_VERBOSE("transport stack example start")

    ImprintDefaultSetup memory;

    imprintDefaultSetupInit(&memory, 16 * 1024 * 1024);

    struct ImprintAllocator* allocator = &memory.tagAllocator.info;
    struct ImprintAllocatorWithFree* allocatorWithFree = &memory.slabAllocator.info;

    TransportStackMulti multi;
    Clog multiLog;
    multiLog.config = &g_clog;
    multiLog.constantPrefix = "multi";

    transportStackMultiInit(&multi, allocator, allocatorWithFree, TransportStackModeLocalUdp, multiLog);
    transportStackMultiListen(&multi, "", 27003);

    TransportStackSingle single;

    Clog singleLog;
    singleLog.config = &g_clog;
    singleLog.constantPrefix = "single";

    transportStackSingleInit(&single, allocator, allocatorWithFree, TransportStackModeLocalUdp, singleLog);
    transportStackSingleConnect(&single, "127.0.0.1", 27003);
#define IN_BUF_SIZE_OCTET_COUNT (1200U)
    uint8_t targetBuf[IN_BUF_SIZE_OCTET_COUNT];

    uint16_t sendTick = 0;
    for (size_t i = 0; i < 50; ++i) {
        usleep(16 * 1000);
        transportStackSingleUpdate(&single);
        int received = datagramTransportReceive(&single.singleTransport, targetBuf, IN_BUF_SIZE_OCTET_COUNT);
        if (received < 0) {
            return received;
        }
        if (transportStackSingleIsConnected(&single)) {

            tc_snprintf(targetBuf, IN_BUF_SIZE_OCTET_COUNT, "Hello %04X", sendTick);
            sendTick++;
            CLOG_INFO("sending '%s' from single to multi transport", targetBuf)

            datagramTransportSend(&single.singleTransport, targetBuf, tc_strlen(targetBuf) + 1);
        }
        transportStackMultiUpdate(&multi);
        int foundConnectionIndex;
        int octetCount = datagramTransportMultiReceiveFrom(&multi.multiTransport, &foundConnectionIndex, targetBuf,
                                                       IN_BUF_SIZE_OCTET_COUNT);
        if (octetCount > 0) {
            CLOG_INFO("received '%s' (octetCount:%d) on multi transport, from single connection %d", targetBuf,
                      octetCount, foundConnectionIndex);
        }
    }

    imprintDefaultSetupDestroy(&memory);
}

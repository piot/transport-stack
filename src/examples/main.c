/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
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
    g_clog.level = CLOG_TYPE_VERBOSE;

    CLOG_VERBOSE("transport stack example start")

    ImprintDefaultSetup memory;

    imprintDefaultSetupInit(&memory, 16 * 1024 * 1024);

    struct ImprintAllocator* allocator = &memory.tagAllocator.info;
    struct ImprintAllocatorWithFree* allocatorWithFree = &memory.slabAllocator.info;

    TransportStackMulti multi;
    Clog multiLog;
    multiLog.config = &g_clog;
    multiLog.constantPrefix = "multi";

    transportStackMultiInit(&multi, TransportStackModeLocalUdp, multiLog);
    transportStackMultiListen(&multi, "", 27003);

    TransportStackSingle single;

    Clog singleLog;
    singleLog.config = &g_clog;
    singleLog.constantPrefix = "single";

    transportStackSingleInit(&single, allocator, allocatorWithFree, TransportStackModeLocalUdp, singleLog);
    transportStackSingleConnect(&single, "127.0.0.1", 27003);

    udpTransportSend(&single.singleTransport, "Hello", 6);

    for (size_t i = 0; i < 2; ++i) {
        CLOG_INFO("waiting...")
        sleep(1);
        transportStackSingleUpdate(&single);
        transportStackMultiUpdate(&multi);
    }

    int foundConnectionIndex;
    uint8_t targetBuf[16];
    const size_t targetBufSize = 16;

    int octetCount = datagramTransportMultiReceive(&multi.multiTransport, &foundConnectionIndex, targetBuf,
                                                   targetBufSize);
    CLOG_INFO("received %d from connection %d", octetCount, foundConnectionIndex);

    imprintDefaultSetupDestroy(&memory);
}

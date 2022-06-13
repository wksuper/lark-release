/*
 * Copyright (c) 2022 Kui Wang
 *
 * This file is part of lark project.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 */

#include <lark/lark.h>
#include <klogging.h>
#include <unistd.h>

#if defined(__APPLE__)
#define SUFIX ".dylib"
#elif defined(_WIN32)
#define SUFIX ".dll"
#else
#define SUFIX ".so"
#endif

// #define TOM_CAT

int main()
{
    const unsigned int rate = 16000;
    const lark::SampleFormat format = lark::SampleFormat_FLOAT;
    const unsigned int chNum = 1;
    const unsigned int frameDuration_ms = 20;
    const lark::samples_t frameSizeInSamples = frameDuration_ms * rate / 1000;

    lark::FIFO *fifo = lark::Lark::Instance().NewFIFO(
                                rate,
                                lark::SamplesToBytes(format, chNum, 1),
                                frameSizeInSamples * 8);
    if (!fifo) {
        KLOGE("Failed to new a FIFO");
        return -1;
    }

    // Create the playback route named RouteA
    lark::Route *playbackRoute = lark::Lark::Instance().NewRoute("RouteA");
    if (!playbackRoute) {
        KLOGE("Failed to create playbackRoute");
        return -1;
    }

    // Create RouteA's blocks
    const char *soFileName = "libblkstreamin" SUFIX;
    lark::Parameters args;
    args.push_back(std::to_string(rate));
    args.push_back(std::to_string(format));
    args.push_back(std::to_string(chNum));
    lark::DataProducer *producer = fifo;
    producer->SetBlocking(true);
    args.push_back(std::to_string((unsigned long)producer));
    lark::Block *blkStreamIn = playbackRoute->NewBlock(soFileName, true, false, args);
    if (!blkStreamIn) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

#ifdef TOM_CAT
    soFileName = "libblkdelay" SUFIX;
    args.clear();
    args.push_back("800000"); // delay 800ms
    lark::Block *blkDelay = playbackRoute->NewBlock(soFileName, false, false, args);
    if (!blkDelay) {
        KLOGE("Failqed to new a block from %s", soFileName);
        return -1;
    }
#endif

    soFileName = "libblksoundtouch" SUFIX;
    lark::Block *blkSoundTouch = playbackRoute->NewBlock(soFileName, false, false);
    if (!blkSoundTouch) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkpaplayback" SUFIX;
    lark::Block *blkPlayback = playbackRoute->NewBlock(soFileName, false, true);
    if (!blkPlayback) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    // Create RouteA's links
#ifdef TOM_CAT
    if (!playbackRoute->NewLink(rate, format, chNum, frameSizeInSamples, blkStreamIn, 0, blkDelay, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!playbackRoute->NewLink(rate, format, chNum, frameSizeInSamples, blkDelay, 0, blkSoundTouch, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
#else
    if (!playbackRoute->NewLink(rate, format, chNum, frameSizeInSamples, blkStreamIn, 0, blkSoundTouch, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
#endif
    if (!playbackRoute->NewLink(rate, format, chNum, frameSizeInSamples, blkSoundTouch, 0, blkPlayback, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }

    // Create the capture route named RouteB
    lark::Route *captureRoute = lark::Lark::Instance().NewRoute("RouteB");
    if (!captureRoute) {
        KLOGE("Failed to create captureRoute");
        return -1;
    }

    soFileName = "libblkpacapture" SUFIX;
    lark::Block *blkCapture = captureRoute->NewBlock(soFileName, true, false);
    if (!blkCapture) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

#ifndef TOM_CAT
    soFileName = "libblksoxeffect" SUFIX;
    args.clear();
    args.push_back("echos");
    lark::Block *blkSoxEchos = captureRoute->NewBlock(soFileName, false, false, args);
    if (!blkSoxEchos) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }
#endif

    soFileName = "libblkformatadapter" SUFIX;
    lark::Block *blkFormatAdapter = captureRoute->NewBlock(soFileName, false, false);
    if (!blkFormatAdapter) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkstreamout" SUFIX;
    args.clear();
    args.push_back(std::to_string(rate));
    args.push_back(std::to_string(format));
    args.push_back(std::to_string(chNum));
    lark::DataConsumer *consumer = fifo;
    consumer->SetBlocking(true);
    args.push_back(std::to_string((unsigned long)consumer));
    lark::Block *blkStreamOut = captureRoute->NewBlock(soFileName, false, true, args);
    if (!blkStreamOut) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

#ifndef TOM_CAT
    if (!captureRoute->NewLink(rate, lark::SampleFormat_S32, chNum, frameSizeInSamples, blkCapture, 0, blkSoxEchos, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!captureRoute->NewLink(rate, lark::SampleFormat_S32, chNum, frameSizeInSamples, blkSoxEchos, 0, blkFormatAdapter, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
#else
    if (!captureRoute->NewLink(rate, lark::SampleFormat_S32, chNum, frameSizeInSamples, blkCapture, 0, blkFormatAdapter, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
#endif
    if (!captureRoute->NewLink(rate, format, chNum, frameSizeInSamples, blkFormatAdapter, 0, blkStreamOut, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }

    // Start() or Stop()
    if (captureRoute->Start() < 0) {
        KLOGE("Failed to start captureRoute");
        return -1;
    }
    usleep(8 * frameDuration_ms * 1000);
    if (playbackRoute->Start() < 0) {
        KLOGE("Failed to start playbackRoute");
        return -1;
    }
    while (1) {
        KLOGA("Press 's' to stop, 'p' to play, 'q' to exit");
again:
        char c;
        if (scanf("%c", &c) != 1)
            break;
        if (c == '\n')
            goto again;
        if (c == 's') {
            playbackRoute->Stop();
        } else if (c == 'p') {
            playbackRoute->Start();
        } else if (c == 'q') {
            break;
        }
    }

    fifo->Shutdown();

    // 5. Automatically release resources

    return 0;
}

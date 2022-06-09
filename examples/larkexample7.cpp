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

#if defined(__APPLE__)
#define SUFIX ".dylib"
#elif defined(_WIN32)
#define SUFIX ".dll"
#else
#define SUFIX ".so"
#endif

int main()
{
    const unsigned int rate = 48000;
    const lark::SampleFormat format = lark::SampleFormat::S16_LE;
    const unsigned int chNum = 2;
    const unsigned int frameDuration_ms = 20;
    const lark::samples_t frameSizeInSamples = frameDuration_ms * rate / 1000;

    // 1. Create the playback route named RouteA
    lark::Route *route = lark::Lark::Instance().NewRoute("RouteA");
    if (!route) {
        KLOGE("Failed to create route");
        return -1;
    }

    // 2. Create RouteA's blocks
    const char *soFileName = "libblkfilereader" SUFIX;
    lark::Parameters args;
    args.push_back("./examples/kanr-48000_16_2.pcm");
    lark::Block *blkFileReader = route->NewBlock(soFileName, true, false, args);
    if (!blkFileReader) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkformatadapter" SUFIX;
    lark::Block *blkFormatAdapter = route->NewBlock(soFileName, false, false);
    if (!blkFormatAdapter) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkdeinterleave" SUFIX;
    lark::Block *blkDeinterleave = route->NewBlock(soFileName, false, false);
    if (!blkDeinterleave) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkmixer" SUFIX;
    args.clear();
    args.push_back("0 0.5");
    args.push_back("1 0.5");
    lark::Block *blkMixer = route->NewBlock(soFileName, false, false, args);
    if (!blkMixer) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblksoxeffect" SUFIX;
    args.clear();
    args.push_back("highpass");
    lark::Block *blkSoxHP = route->NewBlock(soFileName, false, false, args);
    if (!blkSoxHP) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    // soFileName = "libblksoxeffect" SUFIX;
    args.clear();
    args.push_back("lowpass");
    lark::Block *blkSoxLP = route->NewBlock(soFileName, false, false, args);
    if (!blkSoxLP) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkinterleave" SUFIX;
    lark::Block *blkInterleave = route->NewBlock(soFileName, false, false);
    if (!blkInterleave) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkpaplayback" SUFIX;
    lark::Block *blkPAPlayback = route->NewBlock(soFileName, false, true);
    if (!blkPAPlayback) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    // 3. Create RouteA's links
    if (!route->NewLink(rate, format, chNum, frameSizeInSamples, blkFileReader, 0, blkFormatAdapter, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, chNum, frameSizeInSamples, blkFormatAdapter, 0, blkDeinterleave, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, 1, frameSizeInSamples, blkDeinterleave, 0, blkMixer, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, 1, frameSizeInSamples, blkDeinterleave, 1, blkMixer, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, 1, frameSizeInSamples, blkMixer, 0, blkSoxHP, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, 1, frameSizeInSamples, blkMixer, 1, blkSoxLP, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, 1, frameSizeInSamples, blkSoxHP, 0, blkInterleave, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, 1, frameSizeInSamples, blkSoxLP, 0, blkInterleave, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, lark::SampleFormat_S32, chNum, frameSizeInSamples, blkInterleave, 0, blkPAPlayback, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }

    // 4. Start() or Stop()
    if (route->Start() < 0) {
        KLOGE("Failed to start route");
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
            route->Stop();
        } else if (c == 'p') {
            route->Start();
        } else if (c == 'q') {
            break;
        }
    }

    // 5. Automatically release resources

    return 0;
}

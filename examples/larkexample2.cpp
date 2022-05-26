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

int main()
{
    const unsigned int rate = 48000;
    const lark::SampleFormat format = lark::SampleFormat::S16_LE;
    const unsigned int chNum = 2;
    const unsigned int frameDuration_ms = 20;
    const lark::samples_t frameSizeInSamples = frameDuration_ms * rate / 1000;
    const unsigned int GAIN0_L_EPIDX = 0;
    const unsigned int GAIN0_R_EPIDX = 1;
    const unsigned int GAIN1_L_EPIDX = 0;
    const unsigned int GAIN1_R_EPIDX = 1;

    // 1. Create the playback route named RouteA
    lark::Route *route = lark::Lark::Instance().NewRoute("RouteA");
    if (!route) {
        KLOGE("Failed to create route");
        return -1;
    }

    // 2. Create RouteA's blocks
    const char *soFileName = "libblkfilereader.so";
    lark::Parameters args;
    args.push_back("./examples/kanr-48000_16_2.pcm");
    args.push_back(std::to_string(rate));
    args.push_back(std::to_string(format));
    args.push_back(std::to_string(chNum));
    lark::Block *blkFileReader0 = route->NewBlock(soFileName, true, false, args);
    if (!blkFileReader0) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    args.clear();
    args.push_back("./examples/pacificrim-48000_16_2.pcm");
    args.push_back(std::to_string(rate));
    args.push_back(std::to_string(format));
    args.push_back(std::to_string(chNum));
    lark::Block *blkFileReader1 = route->NewBlock(soFileName,  true, false, args);
    if (!blkFileReader1) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkgain.so";
    lark::Block *blkGain0 = route->NewBlock(soFileName, false, false);
    if (!blkGain0) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }
    lark::Block *blkGain1 = route->NewBlock(soFileName, false, false);
    if (!blkGain1) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkmixer.so";
    lark::Block *blkMixerL = route->NewBlock(soFileName, false, false);
    if (!blkMixerL) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }
    lark::Block *blkMixerR = route->NewBlock(soFileName, false, false);
    if (!blkMixerR) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkinterleave.so";
    lark::Block *blkInterleave = route->NewBlock(soFileName, false, false);
    if (!blkInterleave) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkalsaplayback.so";
    lark::Block *blkAlsaPlayback = route->NewBlock(soFileName, false, true);
    if (!blkAlsaPlayback) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    // 3. Create RouteA's links
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkFileReader0, 0, blkGain0, GAIN0_L_EPIDX)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkFileReader0, 1, blkGain0, GAIN0_R_EPIDX)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkFileReader1, 0, blkGain1, GAIN1_L_EPIDX)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkFileReader1, 1, blkGain1, GAIN1_R_EPIDX)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkGain0, GAIN0_L_EPIDX, blkMixerL, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkGain0, GAIN0_R_EPIDX, blkMixerR, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkGain1, GAIN1_L_EPIDX, blkMixerL, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkGain1, GAIN1_R_EPIDX, blkMixerR, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkMixerL, 0, blkInterleave, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 1, frameSizeInSamples, blkMixerR, 0, blkInterleave, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!route->NewLink(rate, format, 2, frameSizeInSamples, blkInterleave, 0, blkAlsaPlayback, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }

    // 4. Start() or Stop()
    if (route->Start() < 0) {
        KLOGE("Failed to start route");
        return -1;
    }
    while (1) {
        KLOGI("Press 's' to stop, 'p' to play, 'q' to exit");
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

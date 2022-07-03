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

class MyDsp {
public:
    MyDsp(const char *playbackPcmName, const char *capturePcmName)
        : m_playbackPcmName(playbackPcmName)
        , m_capturePcmName(capturePcmName)
        , m_playbackRoute(nullptr), m_captureRoute(nullptr) {}
    ~MyDsp() { TeardownRoutes(); }

    int SetupRoutes();
    void TeardownRoutes();

    int StartPlayback();
    int StopPlayback();

    int StartCapture();
    int StopCapture();

private:
    const std::string m_playbackPcmName;
    const std::string m_capturePcmName;
    lark::Route *m_playbackRoute;
    lark::Route *m_captureRoute;

    const unsigned int playbackRate = 48000;
    const unsigned int captureRate = 16000;
    const lark::SampleFormat format = lark::SampleFormat::S16_LE;
    const unsigned int chNum = 2;
    const unsigned int frameDuration_ms = 20;
    const lark::samples_t playbackFrameSizeInSamples = frameDuration_ms * playbackRate / 1000;
    const lark::samples_t captureFrameSizeInSamples = frameDuration_ms * captureRate / 1000;
};

int MyDsp::SetupRoutes()
{
    lark::FIFO *fifo = lark::Lark::Instance().NewFIFO(
                                playbackRate,
                                lark::SamplesToBytes(format, chNum, 1),
                                playbackFrameSizeInSamples * 4);
    if (!fifo) {
        KLOGE("Failed to new a FIFO");
        return -1;
    }

    // 1. Create the capture route named RouteB
    m_captureRoute = lark::Lark::Instance().NewRoute("RouteB");
    if (!m_captureRoute) {
        KLOGE("Failed to create capture route");
        return -1;
    }

    // 2. Create RouteB's blocks
    const char *soFileName = "libblkstreamin.so";
    lark::Parameters args;
    lark::DataProducer *producer = fifo;
    producer->SetBlocking(false);
    args.push_back(std::to_string((unsigned long)producer));
    lark::Block *blkStreamIn = m_captureRoute->NewBlock(soFileName, true, false, args);
    if (!blkStreamIn) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkalsacapture.so";
    args.clear();
    args.push_back(m_capturePcmName);
    lark::Block *blkAlsaCapture = m_captureRoute->NewBlock(soFileName, true, false, args);
    if (!blkAlsaCapture) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkdeinterleave.so";
    lark::Block *blkDeinterleave = m_captureRoute->NewBlock(soFileName, false, false);
    if (!blkDeinterleave) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkdeinterleave.so";
    lark::Block *blkDeinterleaveForStreamIn = m_captureRoute->NewBlock(soFileName, false, false);
    if (!blkDeinterleaveForStreamIn) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkalign.so";
    args.clear();
    args.push_back("200000"); // Configure alignment with 200ms constant delay
    lark::Block *blkAlign = m_captureRoute->NewBlock(soFileName, false, false, args);
    if (!blkAlign) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkspeexresampler.so";
    lark::Block *blkSpeexResampler = m_captureRoute->NewBlock(soFileName, false, false);
    if (!blkSpeexResampler) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkmixer.so";
    args.clear();
    args.push_back("0 0.5");
    args.push_back("1 0.5");
    lark::Block *blkMixerRef = m_captureRoute->NewBlock(soFileName, false, false, args);
    if (!blkMixerRef) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkspeexpreprocessor.so";
    lark::Block *blkPreprocessor = m_captureRoute->NewBlock(soFileName, false, false);
    if (!blkPreprocessor) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkdummybf.so";
    lark::Block *blkBF = m_captureRoute->NewBlock(soFileName, false, false);
    if (!blkBF) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkmixer.so";
    lark::Block *blkMixer = m_captureRoute->NewBlock(soFileName, false, false);
    if (!blkMixer) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkdummykwd.so";
    lark::Block *blkKWD = m_captureRoute->NewBlock(soFileName, false, true);
    if (!blkKWD) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkfilewriter.so";
    args.clear();
    args.push_back("./cap-16000_16_1.pcm");
    lark::Block *blkFileWriter = m_captureRoute->NewBlock(soFileName, false, true, args);
    if (!blkFileWriter) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    // 3. Create RouteB's links

    if (!m_captureRoute->NewLink(captureRate, format, chNum, captureFrameSizeInSamples, blkAlsaCapture, 0, blkAlign, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, chNum, captureFrameSizeInSamples, blkAlign, 0, blkDeinterleave, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkDeinterleave, 0, blkPreprocessor, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkDeinterleave, 1, blkPreprocessor, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(playbackRate, format, chNum, playbackFrameSizeInSamples, blkStreamIn, 0, blkDeinterleaveForStreamIn, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(playbackRate, format, 1, playbackFrameSizeInSamples, blkDeinterleaveForStreamIn, 0, blkSpeexResampler, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(playbackRate, format, 1, playbackFrameSizeInSamples, blkDeinterleaveForStreamIn, 1, blkSpeexResampler, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkSpeexResampler, 0, blkMixerRef, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkSpeexResampler, 1, blkMixerRef, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkMixerRef, 0, blkAlign, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkAlign, 1, blkPreprocessor, 16)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkPreprocessor, 0, blkBF, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkPreprocessor, 1, blkBF, 1)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkBF, 0, blkMixer, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkMixer, 0, blkKWD, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_captureRoute->NewLink(captureRate, format, 1, captureFrameSizeInSamples, blkMixer, 1, blkFileWriter, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }

    // 4. Create the playback route named RouteA
    m_playbackRoute = lark::Lark::Instance().NewRoute("RouteA");
    if (!m_playbackRoute) {
        KLOGE("Failed to create playback route");
        return -1;
    }

    // 5. Create RouteA's blocks
    soFileName = "libblkfilereader.so";
    args.clear();
    args.push_back("./examples/kanr-48000_16_2.pcm");
    lark::Block *blkFileReader = m_playbackRoute->NewBlock(soFileName, true, false, args);
    if (!blkFileReader) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkalsaplayback.so";
    args.clear();
    args.push_back(m_playbackPcmName);
    lark::Block *blkAlsaPlayback = m_playbackRoute->NewBlock(soFileName, false, false, args);
    if (!blkAlsaPlayback) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    soFileName = "libblkstreamout.so";
    args.clear();
    lark::DataConsumer *consumer = fifo;
    consumer->SetBlocking(false);
    args.push_back(std::to_string((unsigned long)consumer));
    lark::Block *blkStreamOut = m_playbackRoute->NewBlock(soFileName, false, true, args);
    if (!blkStreamOut) {
        KLOGE("Failed to new a block from %s", soFileName);
        return -1;
    }

    // 6. Create RouteA's links
    if (!m_playbackRoute->NewLink(playbackRate, format, chNum, playbackFrameSizeInSamples, blkFileReader, 0, blkAlsaPlayback, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    if (!m_playbackRoute->NewLink(playbackRate, format, chNum, playbackFrameSizeInSamples, blkAlsaPlayback, 0, blkStreamOut, 0)) {
        KLOGE("Failed to new a link");
        return -1;
    }
    return 0;
}

void MyDsp::TeardownRoutes()
{
    // 1. Delete the playback route
    if (m_playbackRoute) {
        lark::Lark::Instance().DeleteRoute(m_playbackRoute);
        m_playbackRoute = nullptr;
    }

    // 2. Delete the capture route
    if (m_captureRoute) {
        lark::Lark::Instance().DeleteRoute(m_captureRoute);
        m_captureRoute = nullptr;
    }
}

int MyDsp::StartPlayback()
{
    int ret = m_playbackRoute->Start();
    if (ret < 0) {
        KLOGE("Failed to start playback route");
    }
    return ret;
}

int MyDsp::StopPlayback()
{
    int ret = m_playbackRoute->Stop();
    if (ret < 0) {
        KLOGE("Failed to stop playback route");
    }
    return ret;
}

int MyDsp::StartCapture()
{
    int ret = m_captureRoute->Start();
    if (ret < 0) {
        KLOGE("Failed to start capture route");
    }
    return ret;
}

int MyDsp::StopCapture()
{
    int ret = m_captureRoute->Stop();
    if (ret < 0) {
        KLOGE("Failed to stop capture route");
    }
    return ret;
}

int main(int argc, char *argv[])
{
    const char *playbackPcmName = "default";
    const char *capturePcmName = "default";
    if (argc == 3) {
        playbackPcmName = argv[1];
        capturePcmName = argv[2];
    }
    MyDsp mydsp(playbackPcmName, capturePcmName);

    if (mydsp.SetupRoutes() < 0)
        return -1;

    if (mydsp.StartPlayback() < 0)
        return -1;

    if (mydsp.StartCapture() < 0)
        return -1;

    while (1) {
        KLOGA("Press 's' to stop playback, 'p' to start playback, 'q' to exit");
again:
        int c = getchar();
        if (c == '\n')
            goto again;
        if (c == 's') {
            mydsp.StopPlayback();
        } else if (c == 'p') {
            mydsp.StartPlayback();
        } else if (c == 'q') {
            break;
        }
    }

    mydsp.TeardownRoutes();

    return 0;
}

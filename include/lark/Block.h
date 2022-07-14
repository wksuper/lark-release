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

#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <cstdint>
#include <cstdlib>
#include <map>
#include <string>
#include <lark/TypeDefines.h>
#include <klogging.h>

namespace lark {

#define BLOCK_MAX_IN_EPS   (32)
#define BLOCK_MAX_OUT_EPS  (32)
#define BLKLOGF(fmt, args...) KLOGF("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)
#define BLKLOGE(fmt, args...) KLOGE("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)
#define BLKLOGW(fmt, args...) KLOGW("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)
#define BLKLOGI(fmt, args...) KLOGI("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)
#define BLKLOGD(fmt, args...) KLOGD("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)
#define BLKLOGV(fmt, args...) KLOGV("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)
#define BLKBUG(fmt, args...)  KBUG("%s: %s: " fmt, Block::RouteName(), Block::Name(), ##args)


#define BLKDELAY_PARAMID_DELAY_TIME         (1)
#define BLKSOUNDTOUCH_PARAMID_PITCH         (1)
#define BLKSOUNDTOUCH_PARAMID_TEMPO         (2)
#define BLKSOUNDTOUCH_PARAMID_RATE          (3)
#define BLKGAIN_PARAMID_GAIN                (1)
#define BLKFADEIN_PARAMID_FADING_TIME       (1)
#define BLKFADEOUT_PARAMID_FADING_TIME      (1)
#define BLKFADEOUT_PARAMID_TRIGGER_FADING   (2)
#define BLKSPEEXPREPROCESSOR_PARAMID_BYPASS (1)

class Block;
class Link;
class RouteImpl;


struct FrameBuffer {
    FrameBuffer();

    union {
        void *buf;
        uint8_t *buf8;
        int16_t *buf16;
        int32_t *buf32;
        float *bufFloat;
    };
    SampleFormat format;
    // Normally 'chNum' is 1.
    // If the 'buf' is an interleaved buffer, 'chNum' can be 2 or more
    unsigned int chNum;
    samples_t frameSizeInSamples;

    mutable samples_t availSampleNum;
    unsigned int rate;
    mutable int64_t timestamp;
};

class Endpoint {
public:
    size_t Index() const;
    bool IsActive() const;
    const FrameBuffer &FB() const;

protected:
    Endpoint();
    Link *LINK() { return m_link; }

private:
    size_t m_idx;
    Block *m_block;
    Link *m_link;

    FrameBuffer m_frameBuffer;

    friend class Link;
    friend class Block;
    friend class RouteImpl;
    friend class LarkImpl;
};

class InputEndpoint : public Endpoint {
};

class OutputEndpoint : public Endpoint {
public:
    void Resize(samples_t samples);
    void Resize(samples_t samples, const void *data, int64_t timestamp = -1);
};

class Block {
public:
    // The ioctl() API is used like the Unix ioctl function for customization only
    // Its meaning depends on the block author and varies from block to block
    // The block author needs to take care of the race condition with other APIs like ProcessFrame() etc.
    virtual int ioctl(id_t id, void *data);

protected:
    Block(bool first, bool last);
    virtual ~Block();
    const char *Name() const { return m_name.c_str(); }
    const char *RouteName() const { return m_routeName.c_str(); }

    bool ActiveOutEpsDataIsEmpty() const;
    bool ActiveInEpsDataIsReady() const;
    void ClearActiveInEpsData();

    InputEndpoint &InEp(size_t idx) { return m_inEps[idx]; }
    OutputEndpoint &OutEp(size_t idx) { return m_outEps[idx]; }
    const std::map<size_t, InputEndpoint*> &ActiveInEps() const;
    const std::map<size_t, OutputEndpoint*> &ActiveOutEps() const;

    bool Started() const { return m_started; }

private:
    Block() = delete;
    void SetName(const std::string &routeName, const std::string &name, const std::string *dumpPath);
    void CloseDump();

    virtual const char *Subname() const;

    int BaseOnInEpLinked(size_t epIdx);
    virtual int OnInEpLinked(size_t epIdx);

    void BaseOnInEpUnlinked(size_t epIdx);
    virtual void OnInEpUnlinked(size_t epIdx);

    int BaseOnOutEpLinked(size_t epIdx);
    virtual int OnOutEpLinked(size_t epIdx);

    void BaseOnOutEpUnlinked(size_t epIdx);
    virtual void OnOutEpUnlinked(size_t epIdx);

    int BaseStart();
    virtual int Start();

    // In ProcessFrame(),
    // ActiveInEps() is the input data,
    // ActiveOutEps() is the output data.
    // Important: After processing, don't forget to update FB().availSampleNum
    // in ActiveInEps()'s InputEndpoint and ActiveOutEps()'s OutputEndpoint respectively.
    // Return the actual processed sample number on success or a negative value on failure.
    virtual samples_t ProcessFrame() = 0;
    int CommitFrame();

    int BaseStop();
    virtual int Stop();

    virtual int SetParameter(id_t paramId, const Parameters &params);
    virtual int GetParameter(id_t paramId, Parameters &params);

    void BaseReset();
    virtual void Reset();

    virtual size_t Delay() const;

    InputEndpoint m_inEps[BLOCK_MAX_IN_EPS];
    OutputEndpoint m_outEps[BLOCK_MAX_OUT_EPS];
    std::map<size_t, InputEndpoint*> m_activeInEps;
    std::map<size_t, OutputEndpoint*> m_activeOutEps;

    const bool m_first;
    const bool m_last;

    std::string m_routeName;
    std::string m_name;
    const std::string *m_dumpPath;
    FILE *m_dumpFd[BLOCK_MAX_OUT_EPS];

    bool m_started;

    friend class LarkImpl;
    friend class RouteImpl;
    friend class Link;
};


}

#endif

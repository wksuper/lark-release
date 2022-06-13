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

#ifndef __TYPE_DEFINES_H__
#define __TYPE_DEFINES_H__

#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>

namespace lark {

typedef std::vector<std::string> Parameters;

typedef long samples_t;

enum ErrorType : int {
    E_NOERR = 0,      // Route will continue
    E_NOTREADY = -1,  // This frame will be pending and route will continue
    E_AGAIN = -2,     // Route will continue
    E_NOOUTPUT = -3,  // Route will start from first(s) again

    E_TIMEOUT = -10,  // Route will drop this frame and continue next frames

    E_GENERIC = -20,  // Route will be stopped
    E_EOF = -21,      // Route will be stopped

    E_EXIT = -30
};

enum SampleFormat {
    BYTE = 0,
    S16_LE = 1, S16_BE = 2,
    S32_LE = 3, S32_BE = 4,
    FLOAT_LE = 5, FLOAT_BE = 6
};

static inline bool IsLittleEndian()
{
    uint16_t x = 0x1234;
    return *((uint8_t *)&x) == 0x34;
}

static const enum SampleFormat SampleFormat_S16 = IsLittleEndian() ? SampleFormat::S16_LE : SampleFormat::S16_BE;
static const enum SampleFormat SampleFormat_S32 = IsLittleEndian() ? SampleFormat::S32_LE : SampleFormat::S32_BE;
static const enum SampleFormat SampleFormat_FLOAT = IsLittleEndian() ? SampleFormat::FLOAT_LE : SampleFormat::FLOAT_BE;

static inline const char *FormatToString(SampleFormat format)
{
    static const char *s_tbl[] = {
        "BYTE",
        "S16_LE", "S16_BE",
        "S32_LE", "S32_BE",
        "FLOAT_LE", "FLOAT_BE"
    };
    return s_tbl[format];
}

static inline size_t SamplesToBytes(SampleFormat format, unsigned int chNum, samples_t samples)
{
    switch (format) {
    case SampleFormat::BYTE:
        return samples * chNum * sizeof(uint8_t);
    case SampleFormat::S16_LE:
    case SampleFormat::S16_BE:
        return samples * chNum * sizeof(int16_t);
    case SampleFormat::S32_LE:
    case SampleFormat::S32_BE:
        return samples * chNum * sizeof(int32_t);
    case SampleFormat::FLOAT_LE:
    case SampleFormat::FLOAT_BE:
        return samples * chNum * sizeof(float);
    default:
        return 0;
    }
}

static inline samples_t BytesToSamples(SampleFormat format, unsigned int chNum, size_t bytes)
{
    switch (format) {
    case SampleFormat::BYTE:
        return bytes / chNum / sizeof(uint8_t);
    case SampleFormat::S16_LE:
    case SampleFormat::S16_BE:
        return bytes / chNum / sizeof(int16_t);
    case SampleFormat::S32_LE:
    case SampleFormat::S32_BE:
        return bytes / chNum / sizeof(int32_t);
    case SampleFormat::FLOAT_LE:
    case SampleFormat::FLOAT_BE:
        return bytes / chNum / sizeof(float);
    default:
        return 0;
    }
}

class Block;

class DataProducer {
public:
    DataProducer() : m_blocking(true),  m_block(nullptr) { }
    inline int Produce(void *data, samples_t samples, int64_t *timestamp)
    {
        return Produce(data, samples, m_blocking, timestamp);
    }
    void SetBlocking(bool blocking) { m_blocking = blocking; }
    virtual ~DataProducer() {}

private:
    virtual int Produce(void *data, samples_t samples, bool blocking, int64_t *timestamp) = 0;

    bool m_blocking;
    Block *m_block;

    friend class BlkStreamIn;
    friend class LarkImpl;
};

class DataConsumer {
public:
    DataConsumer() : m_blocking(true), m_block(nullptr) { }
    inline int Consume(const void *data, samples_t samples, int64_t timestamp)
    {
        return Consume(data, samples, m_blocking, timestamp);
    }
    void SetBlocking(bool blocking) { m_blocking = blocking; }
    virtual ~DataConsumer() {}

private:
    virtual int Consume(const void *data, samples_t samples, bool blocking, int64_t timestamp) = 0;

    bool m_blocking;
    Block *m_block;

    friend class BlkStreamOut;
    friend class LarkImpl;
};

class FIFO : public DataProducer, public DataConsumer {
public:
    virtual void Shutdown() = 0;
    virtual ~FIFO() {}

protected:
    const char *Name() const { return m_name.c_str(); }

private:
    std::string m_name;
    friend class LarkImpl;
};

}

#endif

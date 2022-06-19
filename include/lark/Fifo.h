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

#ifndef __FIFO_H__
#define __FIFO_H__

#include <lark/TypeDefines.h>
#include <string>

namespace lark {

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

}  // namespace lark

#endif

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
#include <cassert>
#include <cstring>

class BlkPassthrough : public lark::Block {
public:
    BlkPassthrough(bool first, bool last);

private:
    virtual lark::samples_t ProcessFrame() override final;
};

extern "C" lark::Block *CreateBlock(bool first, bool last, const lark::Parameters &args)
{
    return new BlkPassthrough(first, last);
}

BlkPassthrough::BlkPassthrough(bool first, bool last)
    : Block(first, last)
{
}

lark::samples_t BlkPassthrough::ProcessFrame()
{
    // 1. Check active output endpoints
    if (Block::ActiveOutEps().size() == 0) {
        BLKLOGE("No active output endpoint");
        return lark::E_GENERIC;
    }
    if (!Block::ActiveOutEpsDataIsEmpty()) {
        BLKLOGD("Output has data not consumed");
        return lark::E_NOTREADY;
    }
    // 2. Check active input endpoints
    if (Block::ActiveInEps().size() == 0) {
        BLKLOGE("No active input endpoint");
        return lark::E_GENERIC;
    }
    if (!Block::ActiveInEpsDataIsReady()) {
        // This is normal case, we use KLOGV instead of KLOGD or KLOGE.
        // This happens when the path from the first block to this block's inEpIdx=X
        // is NOT as long as the path from the first block to this block's inEpIdx=Y.
        BLKLOGV("Input data is not ready");
        return lark::E_NOTREADY;
    }

    // 3. Now, active input endpoints have data, active output endpoints are empty

    int ret = lark::E_GENERIC;
    const size_t CHNUM = BLOCK_MAX_IN_EPS < BLOCK_MAX_OUT_EPS
                       ? BLOCK_MAX_IN_EPS : BLOCK_MAX_OUT_EPS;
    for (size_t i = 0; i < CHNUM; ++i) {
        const lark::InputEndpoint &inEp = Block::InEp(i);
        const lark::OutputEndpoint &outEp = Block::OutEp(i);
        const lark::FrameBuffer &inFb = inEp.FB();
        const lark::FrameBuffer &outFb = outEp.FB();
        if (inEp.IsActive() && outEp.IsActive()) {
            // 4. Copy the inputs to the outputs
            assert(inFb.chNum == outFb.chNum);
            assert(inFb.format == outFb.format);
            assert(inFb.frameSizeInSamples == outFb.frameSizeInSamples);
            memcpy(outFb.buf, inFb.buf, SamplesToBytes(inFb.format, inFb.chNum, inFb.availSampleNum));

            // 5. Update inEp's and outEp's availSampleNum
            ret = outFb.availSampleNum = inFb.availSampleNum;
            inFb.availSampleNum = 0;

            // 6. Update outEp's timestamp (optional)
            outFb.timestamp = inFb.timestamp;
        }
    }

    // 7. Clear all the active input endpoints data
    Block::ClearActiveInEpsData();

    return ret;
}

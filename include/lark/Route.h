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

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <lark/TypeDefines.h>

namespace lark {

class Block;
class Link;

class Route {
public:
    virtual Block* NewBlock(const char *soFileName, bool first, bool last, const Parameters &args=Parameters()) = 0;

    virtual Link* NewLink(unsigned int rate, SampleFormat format, unsigned int chNum, samples_t frameSizeInSamples,
                              Block *sourceBlk, size_t sourceBlkOutEpIdx,
                              Block *sinkBlk, size_t sinkBlkInEpIdx) = 0;

    virtual int Start() = 0;

    virtual int SetParameter(Block *blk, id_t paramId, const Parameters &params) = 0;

    virtual int GetParameter(Block *blk, id_t paramId, Parameters &params) = 0;

    virtual int Reset(Block *blk) = 0;

    virtual int Stop() = 0;

    virtual int Delete(void *instance) = 0;

    virtual int DeleteAll() = 0;

    virtual void SetDump(const char *path) = 0;

    struct Action {
        enum Name {
            NEW_BLOCK,
            NEW_LINK,
            START,
            SET_PARAMETER,
            GET_PARAMETER,
            RESET,
            STOP,
            DELETE,
            DELETE_ALL,
            EXIT,
            SET_DUMP
        } name;
        union {
            struct NewBlockData {
                const char *soFileName;
                bool first;
                bool last;
                const Parameters *args;
                Block* ret;
            } newBlockData;

            struct NewLinkData {
                unsigned int rate;
                SampleFormat format;
                unsigned int chNum;
                samples_t frameSizeInSamples;
                Block *sourceBlk;
                size_t sourceBlkOutEpIdx;
                Block *sinkBlk;
                size_t sinkBlkInEpIdx;
                Link* ret;
            } newLinkData;
            struct StartData {
                int ret;
            } startData;
            struct SetParameterData {
                Block *blk;
                id_t paramId;
                const Parameters *params;
                int ret;
            } setParameterData;
            struct GetParameterData {
                Block *blk;
                id_t paramId;
                Parameters *params;
                int ret;
            } getParameterData;
            struct ResetData {
                Block *blk;
                int ret;
            } resetData;
            struct StopData {
                int ret;
            } stopData;
            struct DeletetData {
                void *instance;
                int ret;
            } deleteData;
            struct DeletetAllData {
                int ret;
            } deleteAllData;
            struct DumpData {
                const char *path;
            } dumpData;
        } u;
    };

    virtual void ExecuteActions(Action *actions, size_t n) = 0;


    class Callbacks {
    public:
        virtual void OnStarted(Route *route);
        virtual void OnStopped(Route *route);
        virtual void OnExit(Route *route);
    };
};

}
#endif

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

#ifndef __LARK_H__
#define __LARK_H__

#include <lark/TypeDefines.h>
#include <lark/Block.h>
#include <lark/Route.h>

namespace lark {

class Lark {
public:
    // Return the unique ***lark*** instance.
    static Lark &Instance();

    // Create a route named `name` which be one of the 26 routes "RouteA", "RouteB", ..., "RouteZ".
    // The `cbs` is an optional argument. It is used when something need to be done
    // on route started or on route stopped or on route exits.
    // Note: DO NOT call route APIs(like route->SetParameter() etc.) in these callbacks,
    // otherwise deadlock will be caused.
    virtual Route *NewRoute(const char *name, Route::Callbacks *cbs = nullptr) = 0;

    // Manually delete a route.
    // This is not a mandatory action when the route is not used
    // since all the routes' resources are managered by the ***lark*** instance,
    // and all of them will be deleted automatically when the ***lark*** instance is destroying.
    virtual void DeleteRoute(Route *route) = 0;

    // Create a FIFO which can be used by two threads, one for writing data, one for reading data.
    // As an example, please refer to [larkexample3.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp).
    virtual FIFO *NewFIFO(unsigned int rate, size_t sampleSizeInBytes,
                samples_t bufSizeInSamples, samples_t delayInSamples = 0) = 0;

    // Manually delete a fifo.
    // This is not a mandatory action when the fifo is not used since all the fifos' resources are managered by the ***lark*** instance,
    // and all of them will be deleted automatically when the ***lark*** instance is destroying.
    virtual void DeleteFIFO(FIFO *fifo) = 0;

    virtual ~Lark();
};

}

#endif

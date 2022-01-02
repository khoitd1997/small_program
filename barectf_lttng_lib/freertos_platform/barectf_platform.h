/*
 * Copyright (c) 2015 EfficiOS Inc. and Linux Foundation
 * Copyright (c) 2015-2020 Philippe Proulx <pproulx@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "barectf.h"

#pragma once

inline uint32_t getCurrCpu() { return 0; }

// NOTE: THIS NEED TO BE DEFINED BY USER
struct BarectfExeInfo {
    std::string name = "";
    uint64_t    baseAddr;
    uint64_t    memSize;

    // should be true on Linux but probably would be false on bare metal
    bool isPositionIndependent;
};
extern void barectfGetCurrExeInfo(BarectfExeInfo& info);

// NOTE: Due to the nature of templates all functions in this class need to
// be marked with __attribute__((no_instrument_function))
// this is because these functions are inline and also used in function
// entry and exit callback so they can cause an infinite recursive loop
// other non-inline function doesn't need it because they are already
// compiled with no instrument options but inline functions can still be
// instrumented if the caller app has instrumentation turned on
template <typename T>
class BarectfBaseTrace {
   public:
    T* getStreamCtxPtr() __attribute__((no_instrument_function)) { return &streamCtx; }

    void init(uint8_t* bufAddr, const unsigned int bufSize)
        __attribute__((no_instrument_function)) {
        traceBuffer  = bufAddr;
        traceBufSize = bufSize;
    }
    virtual void finish() __attribute__((no_instrument_function)) {}

    virtual ~BarectfBaseTrace() {}

   protected:
    static uint64_t getClockValueCallback(void* const data)
        __attribute__((no_instrument_function)) {
        (void)(data);
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    }

    uint8_t*     traceBuffer  = nullptr;
    unsigned int traceBufSize = 0;

    T streamCtx = {};
};

class BarectfKernelTrace : virtual public BarectfBaseTrace<barectf_kernel_stream_ctx> {
   public:
    bool init(uint8_t* bufAddr, const unsigned int bufSize);
    void finish();

   private:
    static void openPacketCallback(void* const data);
    static void closePacketCallback(void* const data);
    static int  isBackendFullCallback(void* const data);

    static constexpr barectf_platform_callbacks barectfCallback = {
        .default_clock_get_value = getClockValueCallback,
        .is_backend_full         = isBackendFullCallback,
        .open_packet             = openPacketCallback,
        .close_packet            = closePacketCallback,
    };

    void openPacket();
    void closePacket();
};
class BarectfUserTrace : virtual public BarectfBaseTrace<barectf_user_stream_ctx> {
   public:
    bool init(uint8_t* bufAddr, const unsigned int bufSize);
    void finish();

    // user tracing in lttng/trace compass requires having state dump packets before
    // doing any other kind of events, this command takes care of creating very basic
    // statedump packets for that
    void doBasicStatedump();

   private:
    static void openPacketCallback(void* const data);
    static void closePacketCallback(void* const data);
    static int  isBackendFullCallback(void* const data);

    static bool isStatedumpDone;

    static constexpr barectf_platform_callbacks barectfCallback = {
        .default_clock_get_value = getClockValueCallback,
        .is_backend_full         = isBackendFullCallback,
        .open_packet             = openPacketCallback,
        .close_packet            = closePacketCallback,
    };

    void openPacket();
    void closePacket();
};
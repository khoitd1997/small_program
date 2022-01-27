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

#pragma once

#include "FreeRTOS.h"
#include "task.h"

#include "barectf.h"
#include "barectf_utils.h"

#define USE_GETTIME_CLOCK

struct BarectfExeInfo {
    std::string name = "";
    uint64_t    baseAddr;
    uint64_t    memSize;

    // should be true on Linux but probably would be false on bare metal
    bool isPositionIndependent;
};
// NOTE: THIS NEED TO BE DEFINED BY USER
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

    virtual ~BarectfBaseTrace() {}

   protected:
    static uint64_t getClockValueCallback(void* const data)
        __attribute__((no_instrument_function)) {
#ifdef USE_GETTIME_CLOCK
        (void)(data);
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#else
        double tickCount;

        // TODO: Use a more accurate clock
        // TODO: We probably need to do more xPortIsInsideInterrupt check for other functions as
        // well
        if (xPortIsInsideInterrupt()) {
            tickCount = (double)xTaskGetTickCountFromISR();
        } else {
            tickCount = (double)xTaskGetTickCount();
        }

        return (uint64_t)((tickCount / pdMS_TO_TICKS(1)) * 1000000);
#endif
    }

    uint8_t*     traceBuffer  = nullptr;
    unsigned int traceBufSize = 0;

    T streamCtx = {};
};

/**
 * @brief These objects(both user and kernel trace) ARE NOT THREAD SAFE so if they are to be used
 * by multiple threads must have some kind of protections
 *
 * However, the recommended usage by barectf is for each thread to create their own trace
 * so that they don't have to synchronize with each other. As long as the resulting traces
 * are placed into the same directory after collection, babeltrace/trace_compass will read them as
 * one trace and the only difference is that they will have different name showing in
 * the channel field of the events
 *
 * Usage: Check the platform test for example on how to use, these classes mostly serve as nice
 * wrapper for setting up/tearing down, but when you need to log tracepoints,
 * use the functions defined barectf.h
 *
 */
class BarectfKernelTrace : virtual public BarectfBaseTrace<barectf_kernel_stream_ctx> {
   public:
    bool init(uint8_t* bufAddr, const unsigned int bufSize);
    /**
     * call this when you are done tracing
     * @param isEmpty optional param to check if the packet is empty when closing
     * helpful for deciding whether you should write the trace to file since empty trace seems
     * to confuse trace readers
     */
    void finish(bool* isEmpty = nullptr);

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
    /**
     * call this when you are done tracing
     * @param isEmpty optional param to check if the packet is empty when closing
     * helpful for deciding whether you should write the trace to file since empty trace seems
     * to confuse trace readers
     */
    void finish(bool* isEmpty = nullptr);

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

    // user tracing in lttng/trace compass requires having state dump packets before
    // doing any other kind of events, this command takes care of creating very basic
    // statedump packets for that
    void doBasicStatedump();
};
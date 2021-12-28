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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string_view>

#include "barectf.h"

#pragma once

inline uint32_t getCurrCpu() { return 0; }

template <typename T>
class BarectfBaseTrace {
   public:
    T* getStreamCtxPtr() { return &streamCtx; }

    virtual void finish() {
        // fclose should already flush
        fclose(traceFileFd);
        delete[] barectf_packet_buf(&streamCtx);
    }

   protected:
    static uint64_t getClockValueCallback(void* const data) {
        (void)(data);
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    }

    bool writeToFile() {
        const size_t nmemb = fwrite(
            barectf_packet_buf(&streamCtx), barectf_packet_buf_size(&streamCtx), 1, traceFileFd);

        return nmemb == 1;
    }

    FILE*    traceFileFd = nullptr;
    uint8_t* traceBuffer = nullptr;

    T streamCtx = {};
};

class BarectfKernelTrace : virtual public BarectfBaseTrace<barectf_kernel_stream_ctx> {
   public:
    bool init(const unsigned int bufSize,
              std::string_view   traceFilePath,
              const int          simulateFullBackend,
              const unsigned int fullBackendRandLt,
              const unsigned int fullBackendRandMax);
    void finish();

    void openPacket();
    void closePacket();

   private:
    static void openPacketCallback(void* const data);
    static void closePacketCallback(void* const data);
    static int  isBackendFullCallback(void* const data);

    int          simulateFullBackend;
    unsigned int fullBackendRandLt;
    unsigned int fullBackendRandMax;

    static constexpr barectf_platform_callbacks barectfCallback = {
        .default_clock_get_value = getClockValueCallback,
        .is_backend_full         = isBackendFullCallback,
        .open_packet             = openPacketCallback,
        .close_packet            = closePacketCallback,
    };
};
class BarectfKernelTraceGuard {
   public:
    inline BarectfKernelTraceGuard(BarectfKernelTrace& trace) : myTrace{trace} {
        myTrace.openPacket();
    }
    inline ~BarectfKernelTraceGuard() { myTrace.closePacket(); }

   private:
    BarectfKernelTrace& myTrace;
};

class BarectfUserTrace : virtual public BarectfBaseTrace<barectf_user_stream_ctx> {
   public:
    bool init(const unsigned int bufSize, std::string_view traceFilePath);
    void finish();

    void openPacket();
    void closePacket();

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
};
class BarectfUserTraceGuard {
   public:
    inline BarectfUserTraceGuard(BarectfUserTrace& trace) : myTrace{trace} { myTrace.openPacket(); }
    inline ~BarectfUserTraceGuard() { myTrace.closePacket(); }

   private:
    BarectfUserTrace& myTrace;
};

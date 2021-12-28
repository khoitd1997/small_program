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

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "barectf-platform-linux-fs.h"
#include "barectf.h"

bool BarectfKernelTrace::init(const unsigned int bufSize,
                              std::string_view   traceFilePath,
                              const int          simulateFullBackend,
                              const unsigned int fullBackendRandLt,
                              const unsigned int fullBackendRandMax) {
    this->simulateFullBackend = simulateFullBackend;
    this->fullBackendRandLt   = fullBackendRandLt;
    this->fullBackendRandMax  = fullBackendRandMax;

    traceBuffer = new uint8_t[bufSize];
    if (nullptr == traceBuffer) { goto error; }

    traceFileFd = fopen(traceFilePath.data(), "wb");
    if (nullptr == traceFileFd) { goto error; }

    barectf_init(&streamCtx, traceBuffer, bufSize, barectfCallback, this);

    return true;

error:
    delete[] traceBuffer;
    return false;
}
void BarectfKernelTrace::finish() {
    closePacket();

    BarectfBaseTrace::finish();
}

void BarectfKernelTrace::openPacket() {
    barectf_kernel_stream_open_packet(&streamCtx, getCurrCpu());
}
void BarectfKernelTrace::closePacket() {
    if (barectf_packet_is_open(&streamCtx) && !barectf_packet_is_empty(&streamCtx)) {
        barectf_kernel_stream_close_packet(&streamCtx);
        writeToFile();
    }
}

void BarectfKernelTrace::openPacketCallback(void *const data) {
    BarectfKernelTrace *kernelTrace = static_cast<BarectfKernelTrace *>(data);
    kernelTrace->openPacket();
}
void BarectfKernelTrace::closePacketCallback(void *const data) {
    BarectfKernelTrace *kernelTrace = static_cast<BarectfKernelTrace *>(data);
    kernelTrace->closePacket();
}
int BarectfKernelTrace::isBackendFullCallback(void *const data) {
    BarectfKernelTrace *kernelTrace   = static_cast<BarectfKernelTrace *>(data);
    int                 isBackendFull = 0;

    if (kernelTrace->simulateFullBackend) {
        if (rand() % kernelTrace->fullBackendRandMax < kernelTrace->fullBackendRandLt) {
            isBackendFull = 1;
            goto end;
        }
    }

end:
    return isBackendFull;
}
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

#include <stdexcept>

#include "barectf.h"
#include "barectf_platform.h"
#include "barectf_utils.h"

bool BarectfUserTrace::isStatedumpDone = false;

bool BarectfKernelTrace::init(uint8_t* bufAddr, const unsigned int bufSize) {
    BarectfBaseTrace::init(bufAddr, bufSize);

    barectf_init(&streamCtx, traceBuffer, bufSize, barectfCallback, this);
    openPacket();

    return true;
}
void BarectfKernelTrace::finish(bool* isEmpty) {
    if (isEmpty) { *isEmpty = barectf_packet_is_empty(&streamCtx); }
    closePacket();
}

void BarectfKernelTrace::openPacket() {
    barectf_kernel_stream_open_packet(&streamCtx, getCurrCpu());
}
void BarectfKernelTrace::closePacket() {
    if (barectf_packet_is_open(&streamCtx) && !barectf_packet_is_empty(&streamCtx)) {
        barectf_kernel_stream_close_packet(&streamCtx);
    }
}

void BarectfKernelTrace::openPacketCallback(void* const data) {
    BarectfKernelTrace* kernelTrace = static_cast<BarectfKernelTrace*>(data);
    kernelTrace->openPacket();
}
void BarectfKernelTrace::closePacketCallback(void* const data) {
    BarectfKernelTrace* kernelTrace = static_cast<BarectfKernelTrace*>(data);
    kernelTrace->closePacket();
}
int BarectfKernelTrace::isBackendFullCallback(void* const data) {
    (void)(data);
    return 0;
}

bool BarectfUserTrace::init(uint8_t* bufAddr, const unsigned int bufSize) {
    BarectfBaseTrace::init(bufAddr, bufSize);

    barectf_init(&streamCtx, traceBuffer, bufSize, barectfCallback, this);
    openPacket();

    doBasicStatedump();

    return true;
}
void BarectfUserTrace::finish(bool* isEmpty) {
    if (isEmpty) { *isEmpty = barectf_packet_is_empty(&streamCtx); }
    closePacket();
}
void BarectfUserTrace::openPacket() { barectf_user_stream_open_packet(&streamCtx, getCurrCpu()); }
void BarectfUserTrace::closePacket() {
    if (barectf_packet_is_open(&streamCtx) && !barectf_packet_is_empty(&streamCtx)) {
        barectf_user_stream_close_packet(&streamCtx);
    }
}

void BarectfUserTrace::openPacketCallback(void* const data) {
    BarectfUserTrace* userTrace = static_cast<BarectfUserTrace*>(data);
    userTrace->openPacket();
}
void BarectfUserTrace::closePacketCallback(void* const data) {
    BarectfUserTrace* userTrace = static_cast<BarectfUserTrace*>(data);
    userTrace->closePacket();
}
int BarectfUserTrace::isBackendFullCallback(void* const data) { return 0; }

void BarectfUserTrace::doBasicStatedump() {
    // we only want one statedump ever, so make sure only one guy ever does it
    FreeRTOSCriticalSectionGuard lock{};

    if (isStatedumpDone) { return; }

    BarectfThreadInfo threadInfo;
    getCurrThreadInfo(threadInfo);

    barectf_user_stream_trace_lttng_ust_statedump_start(
        &streamCtx, FreeRtosFixedPid, threadInfo.tid, threadInfo.name);
    barectf_user_stream_trace_lttng_ust_statedump_procname(
        &streamCtx, FreeRtosFixedPid, threadInfo.tid, threadInfo.name, threadInfo.name);

    BarectfExeInfo exeInfo{};
    barectfGetCurrExeInfo(exeInfo);
    if (exeInfo.name.empty()) {
        throw std::runtime_error("barectfGetCurrExeInfo didn't set info properly");
    }

    barectf_user_stream_trace_lttng_ust_statedump_bin_info(&streamCtx,
                                                           FreeRtosFixedPid,
                                                           threadInfo.tid,
                                                           threadInfo.name,
                                                           (uintptr_t)(exeInfo.baseAddr),
                                                           exeInfo.memSize,
                                                           exeInfo.name.c_str(),
                                                           exeInfo.isPositionIndependent ? 1 : 0,
                                                           false,
                                                           false);

    barectf_user_stream_trace_lttng_ust_statedump_end(
        &streamCtx, FreeRtosFixedPid, threadInfo.tid, threadInfo.name);
    isStatedumpDone = true;
}
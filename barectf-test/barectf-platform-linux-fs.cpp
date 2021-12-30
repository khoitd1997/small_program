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

#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "barectf-platform-linux-fs.h"
#include "barectf.h"
#include "barectf_utils.h"

std::mutex BarectfUserTrace::statedumpMutex;
bool       BarectfUserTrace::isStatedumpDone = false;

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

void BarectfKernelTrace::openPacketCallback(void* const data) {
    BarectfKernelTrace* kernelTrace = static_cast<BarectfKernelTrace*>(data);
    kernelTrace->openPacket();
}
void BarectfKernelTrace::closePacketCallback(void* const data) {
    BarectfKernelTrace* kernelTrace = static_cast<BarectfKernelTrace*>(data);
    kernelTrace->closePacket();
}
int BarectfKernelTrace::isBackendFullCallback(void* const data) {
    BarectfKernelTrace* kernelTrace   = static_cast<BarectfKernelTrace*>(data);
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

bool BarectfUserTrace::init(const unsigned int bufSize, std::string_view traceFilePath) {
    traceBuffer = new uint8_t[bufSize];
    if (nullptr == traceBuffer) { goto error; }

    traceFileFd = fopen(traceFilePath.data(), "wb");
    if (nullptr == traceFileFd) { goto error; }

    barectf_init(&streamCtx, traceBuffer, bufSize, barectfCallback, this);
    initialized = true;

    return true;

error:
    delete[] traceBuffer;
    return false;
}
void BarectfUserTrace::finish() {
    closePacket();

    BarectfBaseTrace::finish();
}
void BarectfUserTrace::openPacket() { barectf_user_stream_open_packet(&streamCtx, getCurrCpu()); }
void BarectfUserTrace::closePacket() {
    if (barectf_packet_is_open(&streamCtx) && !barectf_packet_is_empty(&streamCtx)) {
        barectf_user_stream_close_packet(&streamCtx);
        writeToFile();
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

int BarectfUserTrace::dlIterateCallback(dl_phdr_info* info, size_t size, void* data) {
    BarectfUserTrace* userTrace = static_cast<BarectfUserTrace*>(data);
    BarectfThreadInfo threadInfo;
    getCurrThreadInfo(threadInfo);
    std::string objName = info->dlpi_name;
    if (objName.empty()) { objName = getCurrExePath(); }

    std::cout << "Name: " << info->dlpi_name << std::endl;
    std::cout << std::hex << "Base address: 0x" << info->dlpi_addr << std::endl;
    size_t objMemsz = 0;
    for (auto j = 0; j < info->dlpi_phnum; j++) {
        objMemsz += info->dlpi_phdr[j].p_memsz;
        printf("\t\t header %2d: address=%10p\n",
               j,
               (void*)(info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));
    }
    std::cout << std::hex << "Size: 0x" << objMemsz << std::endl;
    std::cout << std::endl;

    barectf_user_stream_trace_lttng_ust_statedump_bin_info(userTrace->getStreamCtxPtr(),
                                                           threadInfo.tid,
                                                           threadInfo.pid,
                                                           threadInfo.name,
                                                           (uintptr_t)(info->dlpi_addr),
                                                           objMemsz,
                                                           objName.c_str(),
                                                           true,
                                                           false,
                                                           false);

    return 0;
}
std::string BarectfUserTrace::getCurrExePath() {
    // readlink doesn't add null terminator so we have to do it ourselves
    char currExeFilePath[100] = {0};
    if (readlink("/proc/self/exe", currExeFilePath, std::size(currExeFilePath) - 1) < 0) {
        throw std::runtime_error("Failed to read /proc/self/exe");
    }
    return currExeFilePath;
}
void BarectfUserTrace::doBasicStatedump() {
    // we only want one statedump ever, so make sure only one guy ever does it
    std::scoped_lock lock{statedumpMutex};

    if (isStatedumpDone) { return; }

    BarectfThreadInfo threadInfo;
    getCurrThreadInfo(threadInfo);

    barectf_user_stream_trace_lttng_ust_statedump_start(
        &streamCtx, threadInfo.tid, threadInfo.pid, threadInfo.name);
    barectf_user_stream_trace_lttng_ust_statedump_procname(
        &streamCtx, threadInfo.tid, threadInfo.pid, threadInfo.name, threadInfo.name);

    std::cout << "dl info: " << std::endl;
    dl_iterate_phdr(BarectfUserTrace::dlIterateCallback, this);

    barectf_user_stream_trace_lttng_ust_statedump_end(
        &streamCtx, threadInfo.tid, threadInfo.pid, threadInfo.name);
    isStatedumpDone = true;
}
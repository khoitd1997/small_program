#include <cassert>
#include <cstdio>
#include <iostream>

/* Include the Linux FS platform header */
#include "barectf-platform-linux-fs.h"

/* Include the barectf public header */
#include "barectf.h"

constexpr size_t TRACE_BUFFER_SIZE_BYTE = 2048;

int main(const int argc, const char *const argv[]) {
    BarectfKernelTrace kernelTrace;
    bool               ret;

    ret = kernelTrace.init(TRACE_BUFFER_SIZE_BYTE, "trace/stream", 0, 0, 0);
    if (!ret) {
        std::cout << "Failed to initialize kernelTrace" << std::endl;
        return -1;
    }

    barectf_kernel_stream_ctx *kernelStreamPtr = kernelTrace.getStreamCtxPtr();

    {
        BarectfTraceGuard traceGuard{kernelTrace};

        barectf_kernel_stream_trace_sched_switch(
            kernelStreamPtr, "task_prev_name", 6, 2, 0, "task_next_name", 12, 3);
        barectf_kernel_stream_trace_sched_switch(
            kernelStreamPtr, "task_next_name", 12, 3, 0, "task_prev_name", 6, 2);
    }

    std::cout << "Finishing kernel trace" << std::endl;
    kernelTrace.finish();

    return 0;
}

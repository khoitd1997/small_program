#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <iostream>

/* Include the Linux FS platform header */
#include "barectf-platform-linux-fs.h"

/* Include the barectf public header */
#include "barectf.h"

constexpr size_t TRACE_BUFFER_SIZE_BYTE = 2048;

struct LttngTaskStruct {
    std::string name;
    int         tid;
    int         prio;
};

LttngTaskStruct task1 = {
    .name = "Task 1",
    .tid  = 6,
    .prio = 2,
};
LttngTaskStruct task2 = {
    .name = "Task 2",
    .tid  = 12,
    .prio = 3,
};

int main(const int argc, const char *const argv[]) {
    BarectfKernelTrace kernelTrace1;
    BarectfKernelTrace kernelTrace2;
    bool               ret;

    ret = kernelTrace1.init(TRACE_BUFFER_SIZE_BYTE, "trace/stream1", 0, 0, 0);
    if (!ret) {
        std::cout << "Failed to initialize kernelTrace1" << std::endl;
        return -1;
    }
    ret = kernelTrace2.init(TRACE_BUFFER_SIZE_BYTE, "trace/stream2", 0, 0, 0);
    if (!ret) {
        std::cout << "Failed to initialize kernelTrace2" << std::endl;
        return -1;
    }

    // NOTE: sched_wakeup doesn't seem necessary to display a nice flow trace
    // multiple stream get displayed into one on trace compass
    // this makes it easy to have one stream per task

    {
        BarectfTraceGuard          traceGuard{kernelTrace1};
        barectf_kernel_stream_ctx *kernelStreamPtr = kernelTrace1.getStreamCtxPtr();

        // barectf_kernel_stream_trace_sched_wakeup(
        //     kernelStreamPtr, task1.name.c_str(), task1.tid, task1.prio, 1);
        sleep(1);
        barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                                 task1.name.c_str(),
                                                 task1.tid,
                                                 task1.prio,
                                                 0,
                                                 task2.name.c_str(),
                                                 task2.tid,
                                                 task2.prio);

        sleep(1);
        // barectf_kernel_stream_trace_sched_wakeup(
        //     kernelStreamPtr, task2.name.c_str(), task2.tid, task2.prio, 1);
        sleep(1);
        barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                                 task2.name.c_str(),
                                                 task2.tid,
                                                 task2.prio,
                                                 0,
                                                 task1.name.c_str(),
                                                 task1.tid,
                                                 task1.prio);
        sleep(1);
        barectf_kernel_stream_trace_sched_wakeup(
            kernelStreamPtr, task2.name.c_str(), task2.tid, task2.prio, getCurrCpu());
    }

    {
        BarectfTraceGuard          traceGuard{kernelTrace2};
        barectf_kernel_stream_ctx *kernelStreamPtr = kernelTrace2.getStreamCtxPtr();

        // barectf_kernel_stream_trace_sched_wakeup(
        //     kernelStreamPtr, task1.name.c_str(), task1.tid, task1.prio, 1);
        sleep(1);
        barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                                 task1.name.c_str(),
                                                 task1.tid,
                                                 task1.prio,
                                                 0,
                                                 task2.name.c_str(),
                                                 task2.tid,
                                                 task2.prio);

        sleep(1);
        // barectf_kernel_stream_trace_sched_wakeup(
        //     kernelStreamPtr, task2.name.c_str(), task2.tid, task2.prio, 1);
        sleep(1);
        barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                                 task2.name.c_str(),
                                                 task2.tid,
                                                 task2.prio,
                                                 0,
                                                 task1.name.c_str(),
                                                 task1.tid,
                                                 task1.prio);
        sleep(1);
        barectf_kernel_stream_trace_sched_wakeup(
            kernelStreamPtr, task2.name.c_str(), task2.tid, task2.prio, getCurrCpu());
    }

    std::cout << "Finishing kernel trace" << std::endl;
    kernelTrace1.finish();
    kernelTrace2.finish();

    return 0;
}

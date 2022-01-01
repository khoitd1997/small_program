#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <iostream>

#include "barectf_function_instrument.h"
#include "barectf_platform.h"
#include "barectf_utils.h"

/* Include the barectf public header */
#include "barectf.h"

constexpr size_t TRACE_BUFFER_SIZE_BYTE = 2048;

struct LttngTaskStruct {
    std::string name;
    int         tid;
    int         prio;
};

const LttngTaskStruct task1 = {
    .name = "Task 1",
    .tid  = 6,
    .prio = 2,
};
const LttngTaskStruct task2 = {
    .name = "Task 2",
    .tid  = 12,
    .prio = 3,
};
constexpr int32_t irqNumber = 21;
const std::string irqName   = "Test IRQ Name";

constexpr uint32_t  GFP_FLAG_KMALLOC      = 3264;
constexpr uintptr_t kmallocAddr           = 0x1400000;
constexpr uintptr_t kmallocBytesRequestd  = 0x1400000;
constexpr uintptr_t kmallocBytesAllocated = 0x1400000;

const std::string lock1Name = "Lock_1";
const uintptr_t   lock1Addr = 0x10000;

constexpr int32_t userThreadVtid = 5;
constexpr int32_t userThreadPid  = 5;
const std::string userThreadName = "User_Thread";

BarectfKernelTrace kernelTrace1;
// BarectfKernelTrace kernelTrace2;
BarectfUserTrace userTrace;

static void doKernelTrace1() {
    // NOTE: sched_wakeup doesn't seem necessary to display a nice flow trace
    // multiple stream get displayed into one on trace compass
    // this makes it easy to have one stream per task

    BarectfKernelTraceGuard    traceGuard{kernelTrace1};
    barectf_kernel_stream_ctx* kernelStreamPtr = kernelTrace1.getStreamCtxPtr();

    // barectf_kernel_stream_trace_sched_wakeup(
    //     kernelStreamPtr, task1.name.c_str(), task1.tid, task1.prio, 1);
    sleep(1);
    barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                             task1.name.c_str(),
                                             task1.tid,
                                             task1.prio,
                                             TASK_RUNNING,
                                             task2.name.c_str(),
                                             task2.tid,
                                             task2.prio);
    usleep(100000);
    barectf_kernel_stream_trace_irq_handler_entry(kernelStreamPtr, irqNumber, irqName.c_str());
    usleep(1000);
    barectf_kernel_stream_trace_irq_handler_exit(kernelStreamPtr, irqNumber, 2);

    sleep(1);
    barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                             task2.name.c_str(),
                                             task2.tid,
                                             task2.prio,
                                             TASK_RUNNING,
                                             task1.name.c_str(),
                                             task1.tid,
                                             task1.prio);
    usleep(100000);
    barectf_kernel_stream_trace_irq_handler_entry(kernelStreamPtr, irqNumber, irqName.c_str());
    usleep(1000);
    barectf_kernel_stream_trace_irq_handler_exit(kernelStreamPtr, irqNumber, 2);

    sleep(1);
    barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                             task1.name.c_str(),
                                             task1.tid,
                                             task1.prio,
                                             TASK_INTERRUPTIBLE,
                                             task2.name.c_str(),
                                             task2.tid,
                                             task2.prio);

    sleep(1);
    barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                             task2.name.c_str(),
                                             task2.tid,
                                             task2.prio,
                                             TASK_UNINTERRUPTIBLE,
                                             task1.name.c_str(),
                                             task1.tid,
                                             task1.prio);

    sleep(1);
    barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
                                             task1.name.c_str(),
                                             task1.tid,
                                             task1.prio,
                                             TASK_STOPPED,
                                             task2.name.c_str(),
                                             task2.tid,
                                             task2.prio);
}
void doKernelTrace2() {
    // {
    //     BarectfKernelTraceGuard    traceGuard{kernelTrace2};
    //     barectf_kernel_stream_ctx *kernelStreamPtr = kernelTrace2.getStreamCtxPtr();

    //     // // barectf_kernel_stream_trace_sched_wakeup(
    //     // //     kernelStreamPtr, task1.name.c_str(), task1.tid, task1.prio, 1);
    //     // sleep(1);
    //     // barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
    //     //                                          task1.name.c_str(),
    //     //                                          task1.tid,
    //     //                                          task1.prio,
    //     //                                          TASK_RUNNING,
    //     //                                          task2.name.c_str(),
    //     //                                          task2.tid,
    //     //                                          task2.prio);

    //     // sleep(1);
    //     // // barectf_kernel_stream_trace_sched_wakeup(
    //     // //     kernelStreamPtr, task2.name.c_str(), task2.tid, task2.prio, 1);
    //     // sleep(1);
    //     // barectf_kernel_stream_trace_sched_switch(kernelStreamPtr,
    //     //                                          task2.name.c_str(),
    //     //                                          task2.tid,
    //     //                                          task2.prio,
    //     //                                          TASK_RUNNING,
    //     //                                          task1.name.c_str(),
    //     //                                          task1.tid,
    //     //                                          task1.prio);
    //     sleep(1);
    //     barectf_kernel_stream_trace_sched_wakeup(
    //         kernelStreamPtr, task2.name.c_str(), task2.tid, task2.prio, getCurrCpu());
    // }
}

void doUserTrace() {
    BarectfUserTraceGuard    traceGuard{userTrace};
    barectf_user_stream_ctx* userStreamPtr = userTrace.getStreamCtxPtr();
    BarectfThreadInfo        threadInfo;
    getCurrThreadInfo(threadInfo);

    userTrace.doBasicStatedump();

    barectf_user_stream_trace_lttng_ust_libc_calloc(
        userStreamPtr, threadInfo.tid, threadInfo.pid, threadInfo.name, 24, 1, 0x192fe40);
    barectf_user_stream_trace_lttng_ust_libc_calloc(
        userStreamPtr, threadInfo.tid, threadInfo.pid, threadInfo.name, 24, 1, 0x192fe40);
    barectf_user_stream_trace_lttng_ust_libc_calloc(
        userStreamPtr, threadInfo.tid, threadInfo.pid, threadInfo.name, 24, 1, 0x192fe40);
    //     userStreamPtr, threadInfo.tid, threadInfo.pid, threadInfo.name, 0x192fe40);
}

static BarectfFunctionInstrument barectfFunctionInstrument(TRACE_BUFFER_SIZE_BYTE,
                                                           getDefaultUserTraceDir() +
                                                               "/user_stream_function_instrument");

int main(const int argc, const char* const argv[]) {
    bool ret;

    std::cout << "Initting kernelTrace1" << std::endl;
    ret = kernelTrace1.init(
        TRACE_BUFFER_SIZE_BYTE, getDefaultKernelTraceDir() + "/kernel_stream1", 0, 0, 0);
    if (!ret) {
        std::cout << "Failed to initialize kernelTrace1" << std::endl;
        return -1;
    }
    // ret = kernelTrace2.init(TRACE_BUFFER_SIZE_BYTE, kernelTraceDir + "/kernel_stream2", 0, 0, 0);
    // if (!ret) {
    //     std::cout << "Failed to initialize kernelTrace2" << std::endl;
    //     return -1;
    // }

    std::cout << "Initting userTrace" << std::endl;
    ret = userTrace.init(TRACE_BUFFER_SIZE_BYTE, getDefaultUserTraceDir() + "/user_stream");
    if (!ret) {
        std::cout << "Failed to initialize userTrace" << std::endl;
        return -1;
    }

    std::cout << "doing kernelTrace1" << std::endl;
    doKernelTrace1();

    std::cout << "doing userTrace" << std::endl;
    doUserTrace();

    std::cout << "Finishing kernel trace" << std::endl;
    kernelTrace1.finish();
    // kernelTrace2.finish();
    userTrace.finish();

    barectfFunctionInstrument.finish();

    return 0;
}

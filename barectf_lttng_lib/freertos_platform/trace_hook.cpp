#include <cstring>
#include <iostream>
#include <stdexcept>

#include "FreeRTOS.h"
#include "task.h"

#include "barectf_platform.h"
#include "barectf_utils.h"
#include "trace_hook.h"

static BarectfKernelTrace hookKernelTrace;

// FreeRTOS doesn't provide a single event that provides both
// previous and next task so we have to remember
static TaskHandle_t prevTaskHandle = nullptr;
// used as the name of prevTask at startup when there is no task to switch from
static const char InvalidTaskName[] = "Invalid_Task";

void taskSwitchedInHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    TaskHandle_t                 nextTaskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    BarectfThreadInfo prevTaskInfo;
    if (prevTaskHandle != nullptr) {
        getThreadInfo(prevTaskHandle, prevTaskInfo);
    } else {
        prevTaskInfo.tid   = -1;
        prevTaskInfo.pid   = -1;
        prevTaskInfo.prio  = -1;
        prevTaskInfo.state = EXIT_DEAD;
        prevTaskInfo.name  = InvalidTaskName;
    }

    BarectfThreadInfo nextTaskInfo;
    getThreadInfo(nextTaskHandle, nextTaskInfo);

    barectf_kernel_stream_trace_sched_switch(hookKernelTrace.getStreamCtxPtr(),
                                             prevTaskInfo.name,
                                             prevTaskInfo.tid,
                                             prevTaskInfo.prio,
                                             prevTaskInfo.state,

                                             nextTaskInfo.name,
                                             nextTaskInfo.tid,
                                             nextTaskInfo.prio);
}
void taskSwitchedOutHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    TaskHandle_t                 taskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    prevTaskHandle = taskHandle;
}

bool traceHookInit(uint8_t* bufAddr, const unsigned int bufSize) {
    if (taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()) {
        throw std::runtime_error(
            "traceHookInit must only be called when FreeRTOS scheduler hasn't started");
        return false;
    }

    if (!hookKernelTrace.init(bufAddr, bufSize)) {
        throw std::runtime_error("failed to initialize hookKernelTrace");
        return false;
    }
    hookKernelTrace.openPacket();

    return true;
}
void traceHookFinish() {
    FreeRTOSCriticalSectionGuard guard{};
    hookKernelTrace.finish();
}
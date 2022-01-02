#include <stdexcept>

#include "FreeRTOS.h"
#include "task.h"

#include "barectf_platform.h"
#include "barectf_utils.h"
#include "trace_hook.h"

static BarectfKernelTrace* hookKernelTrace = nullptr;

// FreeRTOS doesn't provide a single event that provides both
// previous and next task so we have to remember
static TaskHandle_t prevTaskHandle = nullptr;

bool isHookKernelTraceReady() { return hookKernelTrace != nullptr; }

void taskSwitchedInHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    if (!isHookKernelTraceReady()) { return; }
    TaskHandle_t nextTaskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    BarectfThreadInfo prevTaskInfo;
    if (prevTaskHandle != nullptr) {
        getThreadInfo(prevTaskHandle, prevTaskInfo);
    } else {
        prevTaskInfo = preSchdedulerStartThreadInfo;
        // the scheduler has started so the pre scheduler thread
        // will be in an infinite loop forever unless some serious
        // error has happened
        prevTaskInfo.state = TASK_WAKEKILL;
    }

    BarectfThreadInfo nextTaskInfo;
    getThreadInfo(nextTaskHandle, nextTaskInfo);

    barectf_kernel_stream_trace_sched_switch(hookKernelTrace->getStreamCtxPtr(),
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

    if (!isHookKernelTraceReady()) { return; }

    TaskHandle_t taskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    prevTaskHandle = taskHandle;
}

bool traceHookInit(uint8_t* bufAddr, const unsigned int bufSize) {
    if (taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()) {
        throw std::runtime_error(
            "traceHookInit must only be called when FreeRTOS scheduler hasn't started");
        return false;
    }

    if (isHookKernelTraceReady()) {
        throw std::runtime_error("traceHookInit must only be called once");
        return false;
    }

    hookKernelTrace = new BarectfKernelTrace();
    if (hookKernelTrace == nullptr) {
        throw std::runtime_error("failed to new BarectfKernelTrace");
        return false;
    }

    if (!hookKernelTrace->init(bufAddr, bufSize)) {
        throw std::runtime_error("failed to initialize hookKernelTrace");
        return false;
    }

    BarectfThreadInfo prevTaskInfo = {
        .tid = -1, .prio = 0, .state = EXIT_DEAD, .name = "Invalid Task"};

    barectf_kernel_stream_trace_sched_switch(hookKernelTrace->getStreamCtxPtr(),
                                             prevTaskInfo.name,
                                             prevTaskInfo.tid,
                                             prevTaskInfo.prio,
                                             prevTaskInfo.state,

                                             preSchdedulerStartThreadInfo.name,
                                             preSchdedulerStartThreadInfo.tid,
                                             preSchdedulerStartThreadInfo.prio);

    return true;
}
void traceHookFinish() {
    FreeRTOSCriticalSectionGuard guard{};

    if (!isHookKernelTraceReady()) { return; }

    hookKernelTrace->finish();
    delete hookKernelTrace;
    hookKernelTrace = nullptr;
}
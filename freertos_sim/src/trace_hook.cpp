#include <cstring>
#include <iostream>
#include <mutex>
#include <stdexcept>

#include "FreeRTOS.h"
#include "task.h"

#include "barectf-platform-linux-fs.h"
#include "barectf_utils.h"
#include "freertos_utils.h"
#include "trace_hook.h"

static BarectfTaskState getBarectfTaskState(eTaskState freeRTOSState) {
    switch (freeRTOSState) {
        case eReady:
            return TASK_INTERRUPTIBLE;
        case eRunning:
            // NOTE: TASK_RUNNING for Linux only means the task is runnable(so might not be running)
            // so can technically mean either eRunning or eReady
            // but for FreeRTOS specifically, eRunning should be TASK_RUNNING
            return TASK_RUNNING;

        case eBlocked:
            return TASK_INTERRUPTIBLE;
        case eSuspended:
            return TASK_UNINTERRUPTIBLE;
        case eDeleted:
        case eInvalid:
            return TASK_DEAD;
    };
    throw std::runtime_error("Unhandled case getBarectfTaskState");
}
[[maybe_unused]] static BarectfTaskState getBarectfTaskState(TaskHandle_t taskHandle) {
    const eTaskState freeRTOSState = eTaskGetState(taskHandle);
    return getBarectfTaskState(freeRTOSState);
}
[[maybe_unused]] static int32_t getTaskPrio(TaskHandle_t taskHandle) {
    return (int32_t)uxTaskPriorityGet(taskHandle);
}
[[maybe_unused]] static int32_t getTaskId(TaskHandle_t taskHandle) {
    const auto taskId = uxTaskGetTaskNumber(taskHandle);
    std::cout << "Task ID is " << taskId << ", task handle is 0x" << std::hex << taskHandle
              << std::endl;

    return (int32_t)taskId;
}
static void getThreadInfo(TaskHandle_t       taskHandle,
                          BarectfThreadInfo& threadInfo,
                          const char**       outTaskName = nullptr) {
    TaskStatus_t taskStatus;
    vTaskGetInfo(taskHandle, &taskStatus, pdFALSE, eInvalid);

    const int32_t taskId = (int32_t)taskStatus.xTaskNumber;
    // we initialize individual instead of doing aggregate so we avoid zero initialize
    // the string array
    threadInfo.tid   = taskId;
    threadInfo.pid   = taskId;
    threadInfo.prio  = (int32_t)taskStatus.uxCurrentPriority;
    threadInfo.state = getBarectfTaskState(taskStatus.eCurrentState);

    if (outTaskName != nullptr) {
        *outTaskName = taskStatus.pcTaskName;
    } else {
        if (snprintf(threadInfo.name, std::size(threadInfo.name), "%s", taskStatus.pcTaskName) <
            0) {
            throw std::runtime_error("Failed snprintf for threadInfo.name");
        }
    }
}

static BarectfKernelTrace hookKernelTrace;

// FreeRTOS doesn't provide a single event that provides both
// previous and next task so we have to remember
static TaskHandle_t prevTaskHandle = nullptr;
// used as the name of prevTask at startup when there is no task to switch from
static const char InvalidTaskName[] = "Invalid_Task";

void taskSwitchedInHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    TaskHandle_t                 nextTaskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    const char*       prevTaskName;
    BarectfThreadInfo prevTaskInfo;
    if (prevTaskHandle != nullptr) {
        getThreadInfo(prevTaskHandle, prevTaskInfo, &prevTaskName);
    } else {
        prevTaskInfo.tid   = -1;
        prevTaskInfo.pid   = -1;
        prevTaskInfo.prio  = -1;
        prevTaskInfo.state = EXIT_DEAD;
        prevTaskName       = InvalidTaskName;
    }

    const char*       nextTaskName;
    BarectfThreadInfo nextTaskInfo;
    getThreadInfo(nextTaskHandle, nextTaskInfo, &nextTaskName);

    barectf_kernel_stream_trace_sched_switch(hookKernelTrace.getStreamCtxPtr(),
                                             prevTaskName,
                                             prevTaskInfo.tid,
                                             prevTaskInfo.prio,
                                             prevTaskInfo.state,

                                             nextTaskName,
                                             nextTaskInfo.tid,
                                             nextTaskInfo.prio);
}
void taskSwitchedOutHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    TaskHandle_t                 taskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    prevTaskHandle = taskHandle;
}

bool traceHookInit(const unsigned int bufSize, std::string_view traceFilePath) {
    if (taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()) {
        throw std::runtime_error(
            "traceHookInit must only be called when FreeRTOS scheduler hasn't started");
        return false;
    }

    if (!hookKernelTrace.init(bufSize, traceFilePath, false, false, false)) {
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
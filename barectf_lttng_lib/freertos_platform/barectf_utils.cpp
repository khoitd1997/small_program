#include "barectf_utils.h"

#include <unistd.h>
#include <iostream>
#include <stdexcept>

#include "FreeRTOS.h"

BarectfTaskState getBarectfTaskState(eTaskState freeRTOSState) {
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
BarectfTaskState getBarectfTaskState(TaskHandle_t taskHandle) {
    const eTaskState freeRTOSState = eTaskGetState(taskHandle);
    return getBarectfTaskState(freeRTOSState);
}
int32_t getTaskPrio(TaskHandle_t taskHandle) { return (int32_t)uxTaskPriorityGet(taskHandle); }
int32_t getTaskId(TaskHandle_t taskHandle) {
    const auto taskId = uxTaskGetTaskNumber(taskHandle);
    std::cout << "Task ID is " << taskId << ", task handle is 0x" << std::hex << taskHandle
              << std::endl;

    return (int32_t)taskId;
}
void getThreadInfo(TaskHandle_t taskHandle, BarectfThreadInfo& threadInfo) {
    if (taskHandle == nullptr) { throw std::runtime_error("taskHandle is null in getThreadInfo"); }
    TaskStatus_t taskStatus;
    vTaskGetInfo(taskHandle, &taskStatus, pdFALSE, eInvalid);

    const int32_t taskId = (int32_t)taskStatus.xTaskNumber;
    // we initialize individual instead of doing aggregate so we avoid zero initialize
    // the string array
    threadInfo.tid   = taskId;
    threadInfo.prio  = (int32_t)taskStatus.uxCurrentPriority;
    threadInfo.state = getBarectfTaskState(taskStatus.eCurrentState);
    threadInfo.name  = taskStatus.pcTaskName;
}
void getCurrThreadInfo(BarectfThreadInfo& threadInfo) {
    // scheduler hasn't run yet so we can't use xTaskGetCurrentTaskHandle
    // and there should only be one task running anyway
    if (taskSCHEDULER_NOT_STARTED == xTaskGetSchedulerState()) {
        threadInfo = preSchdedulerStartThreadInfo;
        // if a thread is asking about itself, then it must be running
        threadInfo.state = TASK_RUNNING;
        return;
    }
    getThreadInfo(xTaskGetCurrentTaskHandle(), threadInfo);
}

#ifdef __linux
#include <mutex>
static std::recursive_mutex criticalSectionMutex;
#endif

// NOTE: the taskENTER_CRITICAL port on Linux doesn't seem to work super well since
// other tasks can still be run when we are in a critical section
// so use mutex when on Linux
FreeRTOSCriticalSectionGuard::FreeRTOSCriticalSectionGuard() {
#ifdef __linux
    criticalSectionMutex.lock();
#else
    taskENTER_CRITICAL();
#endif
}
FreeRTOSCriticalSectionGuard::~FreeRTOSCriticalSectionGuard() {
#ifdef __linux
    criticalSectionMutex.unlock();
#else
    taskEXIT_CRITICAL();
#endif
}
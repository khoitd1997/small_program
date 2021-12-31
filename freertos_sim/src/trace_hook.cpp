#include <iostream>
#include <mutex>
#include <stdexcept>

#include "FreeRTOS.h"
#include "task.h"

#include "barectf-platform-linux-fs.h"
#include "barectf_utils.h"
#include "freertos_utils.h"
#include "trace_hook.h"

static BarectfKernelTrace hookKernelTrace;

// FreeRTOS doesn't provide a single event that provides both
// previous and next task so we have to remember
static BarectfThreadInfo prevTask;

void taskSwitchedInHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    TaskHandle_t                 taskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    std::cout << "Task " << pcTaskGetName(taskHandle) << " is switched in" << std::endl;
}
void taskSwitchedOutHook(void* taskHandleVoidPtr) {
    FreeRTOSCriticalSectionGuard guard{};
    TaskHandle_t                 taskHandle = static_cast<TaskHandle_t>(taskHandleVoidPtr);

    std::cout << "Task " << pcTaskGetName(taskHandle) << " is switched out" << std::endl;
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
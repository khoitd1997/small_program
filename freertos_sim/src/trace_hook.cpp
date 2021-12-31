#include <iostream>
#include <mutex>
#include <stdexcept>

#include "FreeRTOS.h"
#include "task.h"

#include "barectf-platform-linux-fs.h"
#include "freertos_utils.h"
#include "trace_hook.h"

static BarectfKernelTrace hookKernelTrace;

void taskSwitchedInHook(char* taskName) {
    FreeRTOSCriticalSectionGuard guard{};
    std::cout << "Task " << taskName << " is switched in" << std::endl;
}
void taskSwitchedOutHook(char* taskName) {
    FreeRTOSCriticalSectionGuard guard{};
    std::cout << "Task " << taskName << " is switched out" << std::endl;
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
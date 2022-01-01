#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "FreeRTOS.h"
#include "task.h"

// does not count the null terminator
constexpr size_t MAX_THREAD_NAME_STR_SIZE = 50;

// docs about some of the states:
// https://titanwolf.org/Network/Articles/Article?AID=96564624-e174-4fa9-bfd1-89180740f29e
enum BarectfTaskState : int64_t {
    TASK_RUNNING         = 0,
    TASK_INTERRUPTIBLE   = 1,
    TASK_UNINTERRUPTIBLE = 2,
    TASK_STOPPED         = 4,
    TASK_TRACED          = 8,
    EXIT_DEAD            = 16,
    EXIT_ZOMBIE          = 32,
    TASK_PARKED          = 64,
    TASK_DEAD            = 128,
    TASK_WAKEKILL        = 256,
    TASK_WAKING          = 512,
    TASK_NOLOAD          = 1024,

    // were just created by kernel, not in runqueue yet
    TASK_NEW = 2048,

    TASK_STATE_MAX = 4096,
};

struct BarectfThreadInfo {
    int32_t tid;
    int32_t pid;

    int32_t          prio;
    BarectfTaskState state;

    // freeRTOS just get a pointer back when getting the name
    const char* name;
};

BarectfTaskState                  getBarectfTaskState(eTaskState freeRTOSState);
[[maybe_unused]] BarectfTaskState getBarectfTaskState(TaskHandle_t taskHandle);

[[maybe_unused]] int32_t getTaskPrio(TaskHandle_t taskHandle);
[[maybe_unused]] int32_t getTaskId(TaskHandle_t taskHandle);

void getThreadInfo(TaskHandle_t taskHandle, BarectfThreadInfo& threadInfo);
void getCurrThreadInfo(BarectfThreadInfo& threadInfo);

class FreeRTOSCriticalSectionGuard {
   public:
    inline FreeRTOSCriticalSectionGuard() { taskENTER_CRITICAL(); }
    inline ~FreeRTOSCriticalSectionGuard() { taskEXIT_CRITICAL(); }
};
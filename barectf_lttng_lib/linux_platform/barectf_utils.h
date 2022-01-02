#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

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

    char name[MAX_THREAD_NAME_STR_SIZE];
};

bool getCurrThreadInfo(BarectfThreadInfo& out);

inline std::string getDefaultTraceRootDir() { return std::getenv("TRACE_ROOT_DIR"); }
inline std::string getDefaultKernelTraceDir() {
    return getDefaultTraceRootDir() + "/barectf_kernel_trace";
}
inline std::string getDefaultUserTraceDir() {
    return getDefaultTraceRootDir() + "/barectf_user_trace";
}
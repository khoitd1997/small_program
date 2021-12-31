#include "barectf_utils.h"

#include <pthread.h>
#include <unistd.h>
#include <stdexcept>

bool getCurrThreadInfo(BarectfThreadInfo& out) {
    sched_param param;
    int         policy;
    if (pthread_getschedparam(pthread_self(), &policy, &param)) {
        throw std::runtime_error("Failed pthread_getschedparam");
        return false;
    }

    // TODO: In the future we will need to get these info differently
    // for different OS
    out = {
        .tid = static_cast<int32_t>(gettid()),
        .pid = static_cast<int32_t>(getpid()),

        .prio = param.sched_priority,

        // NOTE: This is only temporary stub, should change when out of testign
        .state = TASK_RUNNING,
    };

    if (0 != pthread_getname_np(pthread_self(), out.name, std::size(out.name))) {
        // TODO: Might want to remove this in the future
        throw std::runtime_error("Failed to get thread name");
        return false;
    }
    return true;
}
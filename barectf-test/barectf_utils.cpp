#include "barectf_utils.h"

#include <pthread.h>
#include <unistd.h>
#include <stdexcept>

bool getCurrThreadInfo(BarectfThreadInfo& out) {
    // TODO: In the future we will need to get these info differently
    // for different OS
    out = {
        .tid = static_cast<int32_t>(gettid()),
        .pid = static_cast<int32_t>(getpid()),
    };

    if (0 != pthread_getname_np(pthread_self(), out.name, std::size(out.name))) {
        // TODO: Might want to remove this in the future
        throw std::runtime_error("Failed to get thread name");
        return false;
    }
    return true;
}
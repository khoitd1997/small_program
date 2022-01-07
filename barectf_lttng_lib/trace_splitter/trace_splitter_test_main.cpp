#include <iostream>

#include <babeltrace/babeltrace.h>
#include <babeltrace/ctf/events.h>
#include <babeltrace/ctf/iterator.h>

// the ctf format is registered when the library is dynamically loaded
// but compiler might not link with the babeltrace-ctf library at all
// if we don't use any symbols from there so call a dummy hook so
// the library gets linked

extern "C" void bt_ctf_hook(void);

int main() {
    bt_ctf_hook();

    bt_context* babelTraceContext = bt_context_create();
    if (nullptr == babelTraceContext) {
        std::cout << "Failed to create trace context" << std::endl;
        return -1;
    }

    int handleId = bt_context_add_trace(babelTraceContext,
                                        "/home/kd/small_program/lttng_tracing/trace_output/kernel",
                                        "ctf",
                                        nullptr,
                                        nullptr,
                                        nullptr);
    if (handleId < 0) {
        bt_context_put(babelTraceContext);
        std::cout << "Failed to add context!!" << std::endl;
        return -1;
    }

    bt_iter_pos* beginPos = bt_iter_create_time_pos(NULL, 0);
    beginPos->type        = BT_SEEK_BEGIN;

    bt_ctf_iter* iter = bt_ctf_iter_create(babelTraceContext, beginPos, nullptr);
    if (!iter) {
        std::cout << "Failed to create iter!!" << std::endl;
        return -1;
    }

    bt_ctf_event* ctf_event;
    while ((ctf_event = bt_ctf_iter_read_event(iter))) {
        std::cout << "Event name is " << bt_ctf_event_name(ctf_event) << std::endl;

        auto ret = bt_iter_next(bt_ctf_get_iter(iter));
        if (ret == -ERANGE) {
            std::cout << "Truncated packet detected!!" << std::endl;
            return -1;
        } else if (ret < 0) {
            break;
        }
    }

    return 0;
}
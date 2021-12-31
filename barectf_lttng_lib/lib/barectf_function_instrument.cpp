#include <iostream>

#include "barectf_function_instrument.h"
#include "barectf_utils.h"

static BarectfUserTrace* userFunctionInstrumentTracePtr = nullptr;
// TODO: Using mutex probably works on OS like Linux but for RTOS with IRQ
// this will need to be implemented differently using something like critical section
static std::mutex* userTraceMutexPtr = nullptr;

static bool isFunctionInstrumentReady() {
    return userTraceMutexPtr != nullptr && userFunctionInstrumentTracePtr != nullptr;
}

void __cyg_profile_func_enter(void* thisFunction, void* callSite) {
    if (!isFunctionInstrumentReady()) { return; }
    std::scoped_lock lock(*userTraceMutexPtr);

    BarectfThreadInfo threadInfo;
    getCurrThreadInfo(threadInfo);

    barectf_user_stream_trace_lttng_ust_cyg_profile_func_entry(
        userFunctionInstrumentTracePtr->getStreamCtxPtr(),
        threadInfo.tid,
        threadInfo.pid,
        threadInfo.name,
        (uint64_t)(thisFunction),
        (uint64_t)(callSite));
}

void __cyg_profile_func_exit(void* thisFunction, void* callSite) {
    if (!isFunctionInstrumentReady()) { return; }
    std::scoped_lock lock(*userTraceMutexPtr);

    BarectfThreadInfo threadInfo;
    getCurrThreadInfo(threadInfo);

    barectf_user_stream_trace_lttng_ust_cyg_profile_func_exit(
        userFunctionInstrumentTracePtr->getStreamCtxPtr(),
        threadInfo.tid,
        threadInfo.pid,
        threadInfo.name,
        (uint64_t)(thisFunction),
        (uint64_t)(callSite));
}

BarectfFunctionInstrument::BarectfFunctionInstrument(const unsigned int bufSize,
                                                     std::string_view   traceFilePath) {
    // need to lock to ensure that the variable isFunctionInstrumentReady is only set after all
    // operations are finished, otw, memory reordering might set it before we are done with other
    // things
    std::scoped_lock lock(userTraceMutex);
    std::cout << "initting BarectfFunctionInstrument" << std::endl;
    if (!userFunctionInstrumentTrace.init(bufSize, traceFilePath)) {
        throw std::runtime_error("Failed to initialize function instrument");
    }
    userFunctionInstrumentTrace.openPacket();
    userFunctionInstrumentTrace.doBasicStatedump();

    // we shouldn't be ready at this time unless we are instantiating more than one instance
    // of BarectfFunctionInstrument which should not be done
    if (isFunctionInstrumentReady()) {
        throw std::runtime_error("BarectfFunctionInstrument is instantiated twice");
    }
    userTraceMutexPtr              = &userTraceMutex;
    userFunctionInstrumentTracePtr = &userFunctionInstrumentTrace;
    std::cout << "done initting BarectfFunctionInstrument" << std::endl;
}

void BarectfFunctionInstrument::finish() {
    std::scoped_lock lock(userTraceMutex);
    userFunctionInstrumentTrace.finish();
}
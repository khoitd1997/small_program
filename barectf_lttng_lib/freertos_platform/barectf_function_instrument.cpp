#include <iostream>

#include "barectf_function_instrument.h"
#include "barectf_utils.h"

static BarectfUserTrace* userFunctionInstrumentTracePtr = nullptr;

static bool isFunctionInstrumentReady() { return userFunctionInstrumentTracePtr != nullptr; }

void __cyg_profile_func_enter(void* thisFunction, void* callSite) {
    FreeRTOSCriticalSectionGuard guard{};
    if (!isFunctionInstrumentReady()) { return; }

    BarectfThreadInfo threadInfo;
    // TODO: Check if this works for IRQ
    getCurrThreadInfo(threadInfo);

    barectf_user_stream_trace_lttng_ust_cyg_profile_func_entry(
        userFunctionInstrumentTracePtr->getStreamCtxPtr(),
        FreeRtosFixedPid,
        threadInfo.tid,
        threadInfo.name,
        (uint64_t)(thisFunction),
        (uint64_t)(callSite));
}

void __cyg_profile_func_exit(void* thisFunction, void* callSite) {
    FreeRTOSCriticalSectionGuard guard{};
    if (!isFunctionInstrumentReady()) { return; }

    BarectfThreadInfo threadInfo;
    getCurrThreadInfo(threadInfo);

    barectf_user_stream_trace_lttng_ust_cyg_profile_func_exit(
        userFunctionInstrumentTracePtr->getStreamCtxPtr(),
        FreeRtosFixedPid,
        threadInfo.tid,
        threadInfo.name,
        (uint64_t)(thisFunction),
        (uint64_t)(callSite));
}

BarectfFunctionInstrument::BarectfFunctionInstrument(uint8_t* bufAddr, const unsigned int bufSize) {
    FreeRTOSCriticalSectionGuard guard{};
    std::cout << "initting BarectfFunctionInstrument" << std::endl;
    if (!userFunctionInstrumentTrace.init(bufAddr, bufSize)) {
        throw std::runtime_error("Failed to initialize function instrument");
    }
    userFunctionInstrumentTrace.openPacket();
    userFunctionInstrumentTrace.doBasicStatedump();

    // we shouldn't be ready at this time unless we are instantiating more than one instance
    // of BarectfFunctionInstrument which should not be done
    if (isFunctionInstrumentReady()) {
        throw std::runtime_error("BarectfFunctionInstrument is instantiated twice");
    }
    userFunctionInstrumentTracePtr = &userFunctionInstrumentTrace;
    std::cout << "done initting BarectfFunctionInstrument" << std::endl;
}

void BarectfFunctionInstrument::finish() {
    FreeRTOSCriticalSectionGuard guard{};
    userFunctionInstrumentTrace.finish();
    userFunctionInstrumentTracePtr = nullptr;
}
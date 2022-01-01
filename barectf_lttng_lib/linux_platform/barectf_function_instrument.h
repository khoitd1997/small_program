#pragma once

#include <stdio.h>
#include <mutex>
#include <string_view>
#include "barectf_platform.h"

// mark extern "C" so that compiler can find it
#ifdef __cplusplus
extern "C" {
#endif

void __cyg_profile_func_enter(void* thisFunction, void* callSite)
    __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void* thisFunction, void* callSite)
    __attribute__((no_instrument_function));

#ifdef __cplusplus
}
#endif

class BarectfFunctionInstrument {
   public:
    BarectfFunctionInstrument(const unsigned int bufSize, std::string_view traceFilePath)
        __attribute__((no_instrument_function));
    void finish() __attribute__((no_instrument_function));

   private:
    BarectfUserTrace userFunctionInstrumentTrace;
    // TODO: Using mutex probably works on OS like Linux but for RTOS with IRQ
    // this will need to be implemented differently using something like critical section
    std::mutex userTraceMutex;
};
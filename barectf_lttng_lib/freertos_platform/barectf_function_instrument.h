#pragma once

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
    BarectfFunctionInstrument(uint8_t* bufAddr, const unsigned int bufSize)
        __attribute__((no_instrument_function));
    void finish() __attribute__((no_instrument_function));

   private:
    BarectfUserTrace userFunctionInstrumentTrace;
};
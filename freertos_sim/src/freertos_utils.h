#pragma once

#include "FreeRTOS.h"
#include "task.h"

class FreeRTOSCriticalSectionGuard {
   public:
    inline FreeRTOSCriticalSectionGuard() { taskENTER_CRITICAL(); }
    inline ~FreeRTOSCriticalSectionGuard() { taskEXIT_CRITICAL(); }
};
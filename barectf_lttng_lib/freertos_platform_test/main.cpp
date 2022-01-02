/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/******************************************************************************
 * This file implements the code that is not demo specific, including the
 * hardware setup and FreeRTOS hook functions.
 *
 *******************************************************************************
 * NOTE: Linux will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Linux port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Linux
 * port for further information:
 * https://freertos.org/FreeRTOS-simulator-for-Linux.html
 *
 *******************************************************************************
 */

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <stdexcept>

#include "FreeRTOS.h"
#include "task.h"

#include "barectf_function_instrument.h"
#include "trace_hook.h"

#include "barectf_utils.h"

inline std::string getDefaultTraceRootDir() { return std::getenv("TRACE_ROOT_DIR"); }
inline std::string getDefaultKernelTraceDir() {
    return getDefaultTraceRootDir() + "/barectf_kernel_trace";
}
inline std::string getDefaultUserTraceDir() {
    return getDefaultTraceRootDir() + "/barectf_user_trace";
}

/* This demo uses heap_3.c (the libc provided malloc() and free()). */

// NOTE: FreeRTOS hook list:
// https://www.freertos.org/a00016.html

/*-----------------------------------------------------------*/

/* When configSUPPORT_STATIC_ALLOCATION is set to 1 the application writer can
 * use a callback function to optionally provide the memory required by the idle
 * and timer tasks.  This is the stack that will be used by the timer task.  It is
 * declared here, as a global, so it can be checked by a test that is implemented
 * in a different file. */
StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

extern "C" {
void vAssertCalled(const char* const pcFileName, unsigned long ulLine) {
    static BaseType_t xPrinted                           = pdFALSE;
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

    (void)xPrinted;
    (void)ulSetToNonZeroInDebuggerToContinue;

    /* Called if an assertion passed to configASSERT() fails.  See
     * http://www.freertos.org/a00110.html#configASSERT for more information. */

    /* Parameters are not used. */
    (void)ulLine;
    (void)pcFileName;

    taskENTER_CRITICAL();
    {
        // TODO: Fill in with trace recording
    }
    taskEXIT_CRITICAL();
}

void vApplicationMallocFailedHook(void) {
    /* vApplicationMallocFailedHook() will only be called if
     * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
     * function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task, queue,
     * timer or semaphore is created.  It is also called by various parts of the
     * demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
     * size of the    heap available to pvPortMalloc() is defined by
     * configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
     * API function can be used to query the size of free heap space that remains
     * (although it does not provide information on how the remaining heap might be
     * fragmented).  See http://www.freertos.org/a00111.html for more
     * information. */
    vAssertCalled(__FILE__, __LINE__);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
     * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
     * task.  It is essential that code added to this hook function never attempts
     * to block in any way (for example, call xQueueReceive() with a block time
     * specified, or call vTaskDelay()).  If application tasks make use of the
     * vTaskDelete() API function to delete themselves then it is also important
     * that vApplicationIdleHook() is permitted to return to its calling function,
     * because it is the responsibility of the idle task to clean up memory
     * allocated by the kernel to any task that has since deleted itself. */

    usleep(15000);
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName) {
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
     * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     * function is called if a stack overflow is detected.  This function is
     * provided as an example only as stack overflow checking does not function
     * when running the FreeRTOS POSIX port. */
    vAssertCalled(__FILE__, __LINE__);
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void) {
    /* This function will be called by each tick interrupt if
     * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
     * added here, but the tick hook is called from an interrupt context, so
     * code must not attempt to block, and only the interrupt safe FreeRTOS API
     * functions can be used (those that end in FromISR()). */
}

void vLoggingPrintf(const char* pcFormat, ...) {
    va_list arg;

    va_start(arg, pcFormat);
    vprintf(pcFormat, arg);
    va_end(arg);
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook(void) {
    /* This function will be called once only, when the daemon task starts to
     * execute    (sometimes called the timer task).  This is useful if the
     * application includes initialisation code that would benefit from executing
     * after the scheduler has been started. */
}

/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                   StackType_t**  ppxIdleTaskStackBuffer,
                                   uint32_t*      pulIdleTaskStackSize) {
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t  uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
     * state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                                    StackType_t**  ppxTimerTaskStackBuffer,
                                    uint32_t*      pulTimerTaskStackSize) {
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
}

std::atomic_int totalTestTask = 0;

static constexpr unsigned int traceHookBufSize = 1024 * 1024 * 5;
static uint8_t*               traceHookBufAddr = static_cast<uint8_t*>(calloc(traceHookBufSize, 1));

static constexpr unsigned int functionTraceBufSize = 1024 * 1024 * 5;
static uint8_t* functionTraceBufAddr = static_cast<uint8_t*>(calloc(functionTraceBufSize, 1));

#define ENABLE_FUNCTION_TRACE

#ifdef ENABLE_FUNCTION_TRACE
static BarectfFunctionInstrument barectfFunctionInstrument(functionTraceBufAddr,
                                                           functionTraceBufSize);
#endif

void writeTraceToFile(void* buf, unsigned int bufSize, std::string_view traceFilePath) {
    FILE* traceFileFd = fopen(traceFilePath.data(), "w");
    if (nullptr == traceFileFd) { throw std::runtime_error("Failed to open trace file"); }

    const size_t nmemb = fwrite(buf, bufSize, 1, traceFileFd);

    if (nmemb != 1) { throw std::runtime_error("Failed to write to trace file"); }
    fflush(traceFileFd);
    fclose(traceFileFd);
}

void basicTask(void* pvParameters) {
    std::cout << "I am task " << pcTaskGetName(nullptr) << "\n";
    int counter = 0;
    for (;;) {
        taskYIELD();
        ++counter;
        if (counter >= 10) { break; }
    }

    --totalTestTask;
    if (totalTestTask == 0) {
        std::cout << "Initiating Exit" << std::endl;
        // vTaskDelay(pdMS_TO_TICKS(100));
        traceHookFinish();
#ifdef ENABLE_FUNCTION_TRACE
        barectfFunctionInstrument.finish();
#endif

        writeTraceToFile(
            traceHookBufAddr, traceHookBufSize, getDefaultKernelTraceDir() + "/trace_hook_stream");
#ifdef ENABLE_FUNCTION_TRACE
        writeTraceToFile(functionTraceBufAddr,
                         functionTraceBufSize,
                         getDefaultUserTraceDir() + "/function_instrument_trace");
#endif
        std::cout << "All task is done, exitting" << std::endl;
        std::exit(0);
    }
    vTaskDelete(nullptr);
}

int main(void) {
    std::cout << "Starting FreeRTOS Posix\n";

    if (!traceHookInit(traceHookBufAddr, traceHookBufSize)) {
        std::cout << "Failed traceHookInit" << std::endl;
        return -1;
    }

    if (pdPASS != xTaskCreate(basicTask, "task_1", 4048, nullptr, 2, nullptr)) {
        std::cout << "Failed to create task_1" << std::endl;
        return -1;
    }
    ++totalTestTask;
    if (pdPASS != xTaskCreate(basicTask, "task_2", 4048, nullptr, 2, nullptr)) {
        std::cout << "Failed to create task_2" << std::endl;
        return -1;
    }
    ++totalTestTask;
    if (pdPASS != xTaskCreate(basicTask, "task_3", 4048, nullptr, 2, nullptr)) {
        std::cout << "Failed to create task_3" << std::endl;
        return -1;
    }
    ++totalTestTask;

    std::cout << "Done with main\n";

    /* Start the scheduler itself. */
    vTaskStartScheduler();

    std::cout << "Failed to start scheduler!!!\n";

    /* Should never get here unless there was not enough heap space to create
     * the idle and other system tasks. */
    return 0;
}
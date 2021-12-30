/*
    Copyright (C) 2016 - 2021 Xilinx, Inc. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software. If you wish to use our Amazon
    FreeRTOS name, please do so in a fair use way that does not cause confusion.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    http://www.FreeRTOS.org
    http://aws.amazon.com/freertos

    1 tab == 4 spaces!
 */

#pragma once

// trace macros docs: https://www.freertos.org/rtos-trace-macros.html

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void taskSwitchedInHook(char* taskName);
void taskSwitchedOutHook(char* taskName);

#define STM_BASE 0xf8000000

#define FREERTOS_EMIT_EVENT(id)
#define FREERTOS_EMIT_DATA(data)

/*
// NOTE: Will need to reenable these once we move to baremetal
#define FREERTOS_STM_CHAN 0
#define FREERTOS_EMIT_EVENT(id) Xil_Out8(STM_BASE + (FREERTOS_STM_CHAN * 0x100), id)
#ifdef EXEC_MODE32
#define FREERTOS_EMIT_DATA(data) Xil_Out32((uint32_t)(STM_BASE + (FREERTOS_STM_CHAN * 0x100) +
0x18), (uint32_t)data) #else #define FREERTOS_EMIT_DATA(data) Xil_Out64((uint64_t)(STM_BASE +
(FREERTOS_STM_CHAN * 0x100) + 0x18), (uint64_t)data) #endif
*/

/* Remove any unused trace macros. */
#ifdef traceSTART
#error "FreeRTOS Trace is already enabled"
#endif

/* Used to perform any necessary initialisation - for example, open a file
into which trace is to be written. */
#define traceSTART()

/* Use to close a trace, for example close a file into which trace has been
written. */
#define traceEND()

/* Called after a task has been selected to run.  pxCurrentTCB holds a pointer
to the task control block of the selected task. */
#define traceTASK_SWITCHED_IN()                           \
    {                                                     \
        TCB_t* pxCurrentTCBCopy = pxCurrentTCB;           \
        taskSwitchedInHook(pxCurrentTCBCopy->pcTaskName); \
    }

/* Called before stepping the tick count after waking from tickless idle
sleep. */
#define traceINCREASE_TICK_COUNT(x)                        \
    {                                                      \
        FREERTOS_EMIT_EVENT(FREERTOS_INCREASE_TICK_COUNT); \
        FREERTOS_EMIT_DATA(x);                             \
    }

/* Called immediately before entering tickless idle. */
#define traceLOW_POWER_IDLE_BEGIN() \
    { FREERTOS_EMIT_EVENT(FREERTOS_LOW_POWER_IDLE_BEGIN); }

/* Called when returning to the Idle task after a tickless idle. */
#define traceLOW_POWER_IDLE_END() \
    { FREERTOS_EMIT_EVENT(FREERTOS_LOW_POWER_IDLE_END); }

/* Called before a task has been selected to run.  pxCurrentTCB holds a pointer
to the task control block of the task being switched out. */
#define traceTASK_SWITCHED_OUT()                           \
    {                                                      \
        TCB_t* pxCurrentTCBCopy = pxCurrentTCB;            \
        taskSwitchedOutHook(pxCurrentTCBCopy->pcTaskName); \
    }

/* Called when a task attempts to take a mutex that is already held by a
lower priority task.  pxTCBOfMutexHolder is a pointer to the TCB of the task
that holds the mutex.  uxInheritedPriority is the priority the mutex holder
will inherit (the priority of the task that is attempting to obtain the
muted. */
#define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority) \
    {                                                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_PRIORITY_INHERIT);                \
        FREERTOS_EMIT_DATA(pxTCBOfMutexHolder);                             \
        FREERTOS_EMIT_DATA(uxInheritedPriority);                            \
    }

/* Called when a task releases a mutex, the holding of which had resulted in
the task inheriting the priority of a higher priority task.
pxTCBOfMutexHolder is a pointer to the TCB of the task that is releasing the
mutex.  uxOriginalPriority is the task's configured (base) priority. */
#define traceTASK_PRIORITY_DISINHERIT(pxTCBOfMutexHolder, uxOriginalPriority) \
    {                                                                         \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_PRIORITY_DISINHERIT);               \
        FREERTOS_EMIT_DATA(pxTCBOfMutexHolder);                               \
        FREERTOS_EMIT_DATA(uxOriginalPriority);                               \
    }

/* Task is about to block because it cannot read from a
queue/mutex/semaphore.  pxQueue is a pointer to the queue/mutex/semaphore
upon which the read was attempted.  pxCurrentTCB points to the TCB of the
task that attempted the read. */
#define traceBLOCKING_ON_QUEUE_RECEIVE(pxQueue)                  \
    {                                                            \
        FREERTOS_EMIT_EVENT(FREERTOS_BLOCKING_ON_QUEUE_RECEIVE); \
        FREERTOS_EMIT_DATA(pxQueue);                             \
    }

/* Task is about to block because it cannot write to a
queue/mutex/semaphore.  pxQueue is a pointer to the queue/mutex/semaphore
upon which the write was attempted.  pxCurrentTCB points to the TCB of the
task that attempted the write. */
#define traceBLOCKING_ON_QUEUE_SEND(pxQueue)                  \
    {                                                         \
        FREERTOS_EMIT_EVENT(FREERTOS_BLOCKING_ON_QUEUE_SEND); \
        FREERTOS_EMIT_DATA(pxQueue);                          \
    }

/* The following event macros are embedded in the kernel API calls. */

#define traceMOVED_TASK_TO_READY_STATE(pxTCB)                    \
    {                                                            \
        FREERTOS_EMIT_EVENT(FREERTOS_MOVED_TASK_TO_READY_STATE); \
        FREERTOS_EMIT_DATA(pxTCB);                               \
    }

#define traceQUEUE_CREATE(pxNewQueue)               \
    {                                               \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_CREATE); \
        FREERTOS_EMIT_DATA(pxNewQueue);             \
        FREERTOS_EMIT_DATA(pxNewQueue->uxLength);   \
    }

#define traceQUEUE_CREATE_FAILED(ucQueueType)              \
    {                                                      \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_CREATE_FAILED); \
        FREERTOS_EMIT_DATA(ucQueueType);                   \
    }

#define traceCREATE_MUTEX(pxNewQueue)                  \
    {                                                  \
        FREERTOS_EMIT_EVENT(FREERTOS_CREATE_MUTEX);    \
        FREERTOS_EMIT_DATA(pxNewQueue);                \
        FREERTOS_EMIT_DATA(pxNewQueue->uxQueueNumber); \
    }

#define traceCREATE_MUTEX_FAILED() \
    { FREERTOS_EMIT_EVENT(FREERTOS_CREATE_MUTEX_FAILED); }

#define traceGIVE_MUTEX_RECURSIVE(pxMutex)                  \
    {                                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_GIVE_MUTEX_RECURSIVE); \
        FREERTOS_EMIT_DATA(pxMutex);                        \
    }

#define traceGIVE_MUTEX_RECURSIVE_FAILED(pxMutex)                  \
    {                                                              \
        FREERTOS_EMIT_EVENT(FREERTOS_GIVE_MUTEX_RECURSIVE_FAILED); \
        FREERTOS_EMIT_DATA(pxMutex);                               \
    }

#define traceTAKE_MUTEX_RECURSIVE(pxMutex)                  \
    {                                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_TAKE_MUTEX_RECURSIVE); \
        FREERTOS_EMIT_DATA(pxMutex);                        \
    }

#define traceTAKE_MUTEX_RECURSIVE_FAILED(pxMutex)                  \
    {                                                              \
        FREERTOS_EMIT_EVENT(FREERTOS_TAKE_MUTEX_RECURSIVE_FAILED); \
        FREERTOS_EMIT_DATA(pxMutex);                               \
    }

#define traceCREATE_COUNTING_SEMAPHORE() \
    { FREERTOS_EMIT_EVENT(FREERTOS_CREATE_COUNTING_SEMAPHORE); }

#define traceCREATE_COUNTING_SEMAPHORE_FAILED() \
    { FREERTOS_EMIT_EVENT(FREERTOS_CREATE_COUNTING_SEMAPHORE_FAILED); }

#define traceQUEUE_SEND(pxQueue)                  \
    {                                             \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_SEND); \
        FREERTOS_EMIT_DATA(pxQueue);              \
    }

#define traceQUEUE_SEND_FAILED(pxQueue)                  \
    {                                                    \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_SEND_FAILED); \
        FREERTOS_EMIT_DATA(pxQueue);                     \
    }

#define traceQUEUE_RECEIVE(pxQueue)                  \
    {                                                \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_RECEIVE); \
        FREERTOS_EMIT_DATA(pxQueue);                 \
    }

#define traceQUEUE_PEEK(pxQueue)                  \
    {                                             \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_PEEK); \
        FREERTOS_EMIT_DATA(pxQueue);              \
    }

#define traceQUEUE_PEEK_FROM_ISR(pxQueue)                  \
    {                                                      \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_PEEK_FROM_ISR); \
        FREERTOS_EMIT_DATA(pxQueue);                       \
    }

#define traceQUEUE_RECEIVE_FAILED(pxQueue)                  \
    {                                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_RECEIVE_FAILED); \
        FREERTOS_EMIT_DATA(pxQueue);                        \
    }

#define traceQUEUE_SEND_FROM_ISR(pxQueue)                  \
    {                                                      \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_SEND_FROM_ISR); \
        FREERTOS_EMIT_DATA(pxQueue);                       \
    }

#define traceQUEUE_SEND_FROM_ISR_FAILED(pxQueue)                  \
    {                                                             \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_SEND_FROM_ISR_FAILED); \
        FREERTOS_EMIT_DATA(pxQueue);                              \
    }

#define traceQUEUE_RECEIVE_FROM_ISR(pxQueue)                  \
    {                                                         \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_RECEIVE_FROM_ISR); \
        FREERTOS_EMIT_DATA(pxQueue);                          \
    }

#define traceQUEUE_RECEIVE_FROM_ISR_FAILED(pxQueue)                  \
    {                                                                \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_RECEIVE_FROM_ISR_FAILED); \
        FREERTOS_EMIT_DATA(pxQueue);                                 \
    }

#define traceQUEUE_PEEK_FROM_ISR_FAILED(pxQueue)                  \
    {                                                             \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_PEEK_FROM_ISR_FAILED); \
        FREERTOS_EMIT_DATA(pxQueue);                              \
    }

#define traceQUEUE_DELETE(pxQueue)                  \
    {                                               \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_DELETE); \
        FREERTOS_EMIT_DATA(pxQueue);                \
    }

#define traceTASK_CREATE(pxNewTCB)

/*
#ifndef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB)                                                   \
    {                                                                                \
        int i, len;                                                                  \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_CREATE);                                   \
        FREERTOS_EMIT_DATA(pxNewTCB);                                                \
        FREERTOS_EMIT_DATA(pxNewTCB->uxPriority);                                    \
        len = strlen(pxNewTCB->pcTaskName);                                          \
        for (i = 0; i < len; i++) { Xil_Out8(0xf8000018, pxNewTCB->pcTaskName[i]); } \
    }
#endif
*/

#define traceTASK_CREATE_FAILED() \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_CREATE_FAILED); }

#define traceTASK_DELETE(pxTaskToDelete)           \
    {                                              \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_DELETE); \
        FREERTOS_EMIT_DATA(pxTaskToDelete);        \
    }

#define traceTASK_DELAY_UNTIL(xTimeToWake)              \
    {                                                   \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_DELAY_UNTIL); \
        FREERTOS_EMIT_DATA(xTimeToWake);                \
    }

#define traceTASK_DELAY() \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_DELAY); }

#define traceTASK_PRIORITY_SET(pxTask, uxNewPriority)    \
    {                                                    \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_PRIORITY_SET); \
        FREERTOS_EMIT_DATA(pxTask);                      \
        FREERTOS_EMIT_DATA(uxNewPriority);               \
    }

#define traceTASK_SUSPEND(pxTaskToSuspend)          \
    {                                               \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_SUSPEND); \
        FREERTOS_EMIT_DATA(pxTaskToSuspend);        \
    }

#define traceTASK_RESUME(pxTaskToResume)           \
    {                                              \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_RESUME); \
        FREERTOS_EMIT_DATA(pxTaskToResume);        \
    }

#define traceTASK_RESUME_FROM_ISR(pxTaskToResume)           \
    {                                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_RESUME_FROM_ISR); \
        FREERTOS_EMIT_DATA(pxTaskToResume);                 \
    }

#ifdef FREERTOS_ENABLE_TIMER_TICK_TRACE
#define traceTASK_INCREMENT_TICK(xTickCount)               \
    {                                                      \
        FREERTOS_EMIT_EVENT(FREERTOS_TASK_INCREMENT_TICK); \
        FREERTOS_EMIT_DATA(xTickCount);                    \
    }
#endif

#define traceTIMER_CREATE(pxNewTimer)                  \
    {                                                  \
        FREERTOS_EMIT_EVENT(FREERTOS_TIMER_CREATE);    \
        FREERTOS_EMIT_DATA(pxNewTimer);                \
        FREERTOS_EMIT_DATA(pxNewTimer->uxTimerNumber); \
    }

#define traceTIMER_CREATE_FAILED() \
    { FREERTOS_EMIT_EVENT(FREERTOS_TIMER_CREATE_FAILED); }

#define traceTIMER_COMMAND_SEND(xTimer, xMessageID, xMessageValueValue, xReturn) \
    {                                                                            \
        FREERTOS_EMIT_EVENT(FREERTOS_TIMER_COMMAND_SEND);                        \
        FREERTOS_EMIT_DATA(xTimer);                                              \
        FREERTOS_EMIT_DATA(xMessageID);                                          \
    }

#define traceTIMER_EXPIRED(pxTimer)                  \
    {                                                \
        FREERTOS_EMIT_EVENT(FREERTOS_TIMER_EXPIRED); \
        FREERTOS_EMIT_DATA(pxTimer);                 \
    }

#define traceTIMER_COMMAND_RECEIVED(pxTimer, xMessageID, xMessageValue) \
    {                                                                   \
        FREERTOS_EMIT_EVENT(FREERTOS_TIMER_COMMAND_RECEIVED);           \
        FREERTOS_EMIT_DATA(pxTimer);                                    \
        FREERTOS_EMIT_DATA(xMessageID);                                 \
    }

#if 0
#define traceMALLOC(pvAddress, uiSize)        \
    {                                         \
        FREERTOS_EMIT_EVENT(FREERTOS_MALLOC); \
        FREERTOS_EMIT_DATA(pvAddress);        \
        FREERTOS_EMIT_DATA(uiSize);           \
    }

#define traceFREE(pvAddress, uiSize)        \
    {                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_FREE); \
        FREERTOS_EMIT_DATA(pvAddress);      \
        FREERTOS_EMIT_DATA(uiSize);         \
    }
#endif

#define traceEVENT_GROUP_CREATE(xEventGroup)              \
    {                                                     \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_CREATE); \
        FREERTOS_EMIT_DATA(xEventGroup);                  \
    }

#define traceEVENT_GROUP_CREATE_FAILED() \
    { FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_CREATE_FAILED); }

#define traceEVENT_GROUP_SYNC_BLOCK(xEventGroup, uxBitsToSet, uxBitsToWaitFor) \
    {                                                                          \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_SYNC_BLOCK);                  \
        FREERTOS_EMIT_DATA(xEventGroup);                                       \
    }

#define traceEVENT_GROUP_SYNC_END(xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred) \
    {                                                                                          \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_SYNC_END);                                    \
        FREERTOS_EMIT_DATA(xEventGroup);                                                       \
    }

#define traceEVENT_GROUP_WAIT_BITS_BLOCK(xEventGroup, uxBitsToWaitFor) \
    {                                                                  \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_WAIT_BITS_BLOCK);     \
        FREERTOS_EMIT_DATA(xEventGroup);                               \
    }

#define traceEVENT_GROUP_WAIT_BITS_END(xEventGroup, uxBitsToWaitFor, xTimeoutOccurred) \
    {                                                                                  \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_WAIT_BITS_END);                       \
        FREERTOS_EMIT_DATA(xEventGroup);                                               \
    }

#define traceEVENT_GROUP_CLEAR_BITS(xEventGroup, uxBitsToClear) \
    {                                                           \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_CLEAR_BITS);   \
        FREERTOS_EMIT_DATA(xEventGroup);                        \
        FREERTOS_EMIT_DATA(uxBitsToClear);                      \
    }

#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(xEventGroup, uxBitsToClear) \
    {                                                                    \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_CLEAR_BITS_FROM_ISR);   \
        FREERTOS_EMIT_DATA(xEventGroup);                                 \
        FREERTOS_EMIT_DATA(uxBitsToClear);                               \
    }

#define traceEVENT_GROUP_SET_BITS(xEventGroup, uxBitsToSet) \
    {                                                       \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_SET_BITS); \
        FREERTOS_EMIT_DATA(xEventGroup);                    \
        FREERTOS_EMIT_DATA(uxBitsToSet);                    \
    }

#define traceEVENT_GROUP_SET_BITS_FROM_ISR(xEventGroup, uxBitsToSet) \
    {                                                                \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_SET_BITS_FROM_ISR); \
        FREERTOS_EMIT_DATA(xEventGroup);                             \
        FREERTOS_EMIT_DATA(uxBitsToSet);                             \
    }

#define traceEVENT_GROUP_DELETE(xEventGroup)              \
    {                                                     \
        FREERTOS_EMIT_EVENT(FREERTOS_EVENT_GROUP_DELETE); \
        FREERTOS_EMIT_DATA(xEventGroup);                  \
    }

#define tracePEND_FUNC_CALL(xFunctionToPend, pvParameter1, ulParameter2, ret) \
    {                                                                         \
        FREERTOS_EMIT_EVENT(FREERTOS_PEND_FUNC_CALL);                         \
        FREERTOS_EMIT_DATA(xFunctionToPend);                                  \
        FREERTOS_EMIT_DATA(pvParameter1);                                     \
    }

#define tracePEND_FUNC_CALL_FROM_ISR(xFunctionToPend, pvParameter1, ulParameter2, ret) \
    {                                                                                  \
        FREERTOS_EMIT_EVENT(FREERTOS_PEND_FUNC_CALL_FROM_ISR);                         \
        FREERTOS_EMIT_DATA(xFunctionToPend);                                           \
        FREERTOS_EMIT_DATA(pvParameter1);                                              \
    }

#define traceQUEUE_REGISTRY_ADD(xQueue, pcQueueName)

/*
#ifndef traceQUEUE_REGISTRY_ADD
#define traceQUEUE_REGISTRY_ADD(xQueue, pcQueueName)                        \
    {                                                                       \
        int i, len;                                                         \
        FREERTOS_EMIT_EVENT(FREERTOS_QUEUE_REGISTRY_ADD);                   \
        FREERTOS_EMIT_DATA(xQueue);                                         \
        len = strlen(pcQueueName);                                          \
        for (i = 0; i < len; i++) { Xil_Out8(0xf8000018, pcQueueName[i]); } \
    }
#endif
*/

#define traceTASK_NOTIFY_TAKE_BLOCK(uxIndexToWait) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY_TAKE_BLOCK); }

#define traceTASK_NOTIFY_TAKE(uxIndexToWait) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY_TAKE); }

#define traceTASK_NOTIFY_WAIT_BLOCK(uxIndexToWait) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY_WAIT_BLOCK); }

#define traceTASK_NOTIFY_WAIT(uxIndexToWait) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY_WAIT); }

#define traceTASK_NOTIFY(uxIndexToNotify) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY); }

#define traceTASK_NOTIFY_FROM_ISR(uxIndexToNotify) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY_FROM_ISR); }

#define traceTASK_NOTIFY_GIVE_FROM_ISR(uxIndexToNotify) \
    { FREERTOS_EMIT_EVENT(FREERTOS_TASK_NOTIFY_GIVE_FROM_ISR); }

#ifdef __cplusplus
}
#endif
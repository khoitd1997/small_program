#pragma once

#include "trace_hook_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FREERTOS_TASK_SWITCHED_IN,
    FREERTOS_INCREASE_TICK_COUNT,
    FREERTOS_LOW_POWER_IDLE_BEGIN,
    FREERTOS_LOW_POWER_IDLE_END,
    FREERTOS_TASK_SWITCHED_OUT,
    FREERTOS_TASK_PRIORITY_INHERIT,
    FREERTOS_TASK_PRIORITY_DISINHERIT,
    FREERTOS_BLOCKING_ON_QUEUE_RECEIVE,
    FREERTOS_BLOCKING_ON_QUEUE_SEND,
    FREERTOS_MOVED_TASK_TO_READY_STATE,
    FREERTOS_QUEUE_CREATE,
    FREERTOS_QUEUE_CREATE_FAILED,
    FREERTOS_CREATE_MUTEX,
    FREERTOS_CREATE_MUTEX_FAILED,
    FREERTOS_GIVE_MUTEX_RECURSIVE,
    FREERTOS_GIVE_MUTEX_RECURSIVE_FAILED,
    FREERTOS_TAKE_MUTEX_RECURSIVE,
    FREERTOS_TAKE_MUTEX_RECURSIVE_FAILED,
    FREERTOS_CREATE_COUNTING_SEMAPHORE,
    FREERTOS_CREATE_COUNTING_SEMAPHORE_FAILED,
    FREERTOS_QUEUE_SEND,
    FREERTOS_QUEUE_SEND_FAILED,
    FREERTOS_QUEUE_RECEIVE,
    FREERTOS_QUEUE_PEEK,
    FREERTOS_QUEUE_PEEK_FROM_ISR,
    FREERTOS_QUEUE_RECEIVE_FAILED,
    FREERTOS_QUEUE_SEND_FROM_ISR,
    FREERTOS_QUEUE_SEND_FROM_ISR_FAILED,
    FREERTOS_QUEUE_RECEIVE_FROM_ISR,
    FREERTOS_QUEUE_RECEIVE_FROM_ISR_FAILED,
    FREERTOS_QUEUE_PEEK_FROM_ISR_FAILED,
    FREERTOS_QUEUE_DELETE,
    FREERTOS_TASK_CREATE,
    FREERTOS_TASK_CREATE_FAILED,
    FREERTOS_TASK_DELETE,
    FREERTOS_TASK_DELAY_UNTIL,
    FREERTOS_TASK_DELAY,
    FREERTOS_TASK_PRIORITY_SET,
    FREERTOS_TASK_SUSPEND,
    FREERTOS_TASK_RESUME,
    FREERTOS_TASK_RESUME_FROM_ISR,
    FREERTOS_TASK_INCREMENT_TICK,
    FREERTOS_TIMER_CREATE,
    FREERTOS_TIMER_CREATE_FAILED,
    FREERTOS_TIMER_COMMAND_SEND,
    FREERTOS_TIMER_EXPIRED,
    FREERTOS_TIMER_COMMAND_RECEIVED,
    FREERTOS_MALLOC,
    FREERTOS_FREE,
    FREERTOS_EVENT_GROUP_CREATE,
    FREERTOS_EVENT_GROUP_CREATE_FAILED,
    FREERTOS_EVENT_GROUP_SYNC_BLOCK,
    FREERTOS_EVENT_GROUP_SYNC_END,
    FREERTOS_EVENT_GROUP_WAIT_BITS_BLOCK,
    FREERTOS_EVENT_GROUP_WAIT_BITS_END,
    FREERTOS_EVENT_GROUP_CLEAR_BITS,
    FREERTOS_EVENT_GROUP_CLEAR_BITS_FROM_ISR,
    FREERTOS_EVENT_GROUP_SET_BITS,
    FREERTOS_EVENT_GROUP_SET_BITS_FROM_ISR,
    FREERTOS_EVENT_GROUP_DELETE,
    FREERTOS_PEND_FUNC_CALL,
    FREERTOS_PEND_FUNC_CALL_FROM_ISR,
    FREERTOS_QUEUE_REGISTRY_ADD,
    FREERTOS_TASK_NOTIFY_TAKE_BLOCK,
    FREERTOS_TASK_NOTIFY_TAKE,
    FREERTOS_TASK_NOTIFY_WAIT_BLOCK,
    FREERTOS_TASK_NOTIFY_WAIT,
    FREERTOS_TASK_NOTIFY,
    FREERTOS_TASK_NOTIFY_FROM_ISR,
    FREERTOS_TASK_NOTIFY_GIVE_FROM_ISR,
} stm_trace_events;

#ifdef __cplusplus
}
#endif
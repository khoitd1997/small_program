cmake_minimum_required(VERSION 3.16)
project(freertos_linux_lib C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FREERTOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOSv202112.00/FreeRTOS")
set(KERNEL_DIR "${FREERTOS_DIR}/Source")

file(GLOB kernel_source ${FREERTOS_DIR}/Source/*.c)

set(posix_port_src
    ${FREERTOS_DIR}/Demo/Common/Minimal/AbortDelay.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/BlockQ.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/blocktim.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/countsem.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/death.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/dynamic.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/EventGroupsDemo.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/flop.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/GenQTest.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/integer.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/IntSemTest.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/MessageBufferAMP.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/MessageBufferDemo.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/PollQ.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/QPeek.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/QueueOverwrite.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/QueueSet.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/QueueSetPolling.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/recmutex.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/semtest.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/StaticAllocation.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/StreamBufferDemo.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/StreamBufferInterrupt.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/TaskNotify.c
    ${FREERTOS_DIR}/Demo/Common/Minimal/TimerDemo.c
    # Memory manager (use malloc() / free() )
    ${KERNEL_DIR}/portable/MemMang/heap_3.c
    # posix port
    ${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
    ${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/port.c)

set(include_dirs
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${KERNEL_DIR}/include"
    "${KERNEL_DIR}/portable/ThirdParty/GCC/Posix"
    "${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils"
    "${FREERTOS_DIR}/Demo/Common/include")

add_library(freertos_lib ${posix_port_src} ${kernel_source})

set(compile_definitions _WINDOWS_)
# need this for trace hook to be inserted
if(DEFINED TRACE_HOOK_MACRO_INCLUDE_DIR)
  set(compile_definitions ${compile_definitions} TRACE_HOOK_MACRO_INCLUDE_DIR)
  set(include_dirs ${include_dirs} "${TRACE_HOOK_MACRO_INCLUDE_DIR}")
endif()

if(NOT DEFINED FREERTOS_CONFIG_INCLUDE_DIR)
  message(FATAL_ERROR "FREERTOS_CONFIG_INCLUDE_DIR must be defined")
endif()
set(include_dirs ${include_dirs} "${FREERTOS_CONFIG_INCLUDE_DIR}")

target_include_directories(freertos_lib PUBLIC ${include_dirs})
target_compile_definitions(freertos_lib PUBLIC ${compile_definitions})
target_link_libraries(freertos_lib PUBLIC pthread)

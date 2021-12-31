cmake_minimum_required(VERSION 3.16)
project(freertos_sim C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../barectf_lttng_lib
                 ${CMAKE_BINARY_DIR}/barectf_lttng_lib)

set(FREERTOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOSv202112.00/FreeRTOS")
set(KERNEL_DIR "${FREERTOS_DIR}/Source")
set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(app_source ${APP_DIR}/main.cpp ${APP_DIR}/console.cpp
               ${APP_DIR}/trace_hook.cpp)
file(GLOB kernel_source ${FREERTOS_DIR}/Source/*.c)

set(demo_lib_src
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
    ${APP_DIR}
    ${KERNEL_DIR}/include
    ${KERNEL_DIR}/portable/ThirdParty/GCC/Posix
    ${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils
    ${FREERTOS_DIR}/Demo/Common/include)

add_executable(freertos_posix_demo ${demo_lib_src} ${app_source}
                                   ${kernel_source})
target_include_directories(freertos_posix_demo PUBLIC ${include_dirs})
target_compile_definitions(freertos_posix_demo PUBLIC _WINDOWS_)
target_compile_options(freertos_posix_demo PUBLIC -g -Wall)
target_link_libraries(freertos_posix_demo PUBLIC pthread barectf_lttng_lib)

add_custom_command(
  TARGET freertos_posix_demo
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:freertos_posix_demo> >
          ${CMAKE_BINARY_DIR}/symbols.txt)

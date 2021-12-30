#include <iostream>

#include "trace_hook.h"

#include "FreeRTOS.h"
#include "task.h"

void taskSwitchedInHook(char* taskName) {
    std::cout << "Task " << taskName << " is switched in" << std::endl;
}
void taskSwitchedOutHook(char* taskName) {
    std::cout << "Task " << taskName << " is switched out" << std::endl;
}
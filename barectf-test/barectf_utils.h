#pragma once

#include <cstddef>
#include <cstdint>

constexpr size_t MAX_THREAD_NAME = 50;

struct BarectfThreadInfo {
    int32_t tid;
    int32_t pid;
    char    name[MAX_THREAD_NAME];
};

bool getCurrThreadInfo(BarectfThreadInfo& out);
#pragma once

#include <string>
#include <string_view>

void splitTrace(uint8_t* traceBuf, size_t traceBufSize, std::string splittedTracePath);
void readTraceFile(const std::string_view traceFilePath, uint8_t** traceBuf, size_t* traceFileSize);
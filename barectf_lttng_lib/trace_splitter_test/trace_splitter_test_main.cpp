#include <iostream>
#include <string_view>

#include "trace_splitter.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <path-to-trace> <path-to-output-folder>"
                  << std::endl;
        return 1;
    }

    const std::string tracePath         = std::string(argv[1]);
    const std::string splittedTracePath = std::string(argv[2]);

    uint8_t* traceBuf = nullptr;
    size_t   traceSize;
    readTraceFile(tracePath, &traceBuf, &traceSize);
    splitTrace(traceBuf, traceSize, splittedTracePath);

    return 0;
}
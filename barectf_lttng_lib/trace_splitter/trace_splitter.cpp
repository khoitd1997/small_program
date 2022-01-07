#include <iostream>
#include <string_view>

#include "trace_splitter.h"

#define _ALIGN(_at_var, _align) \
    do { (_at_var) = ((_at_var) + ((_align)-1)) & -(_align); } while (0)

#define CTF_MAGIC_NUMBER 0xC1FC1FC1

size_t getPacketSizeOffset() {
    size_t offset = 0;
    // the format is based on info generated in the file "metadata"
    offset += 32;      // go past magic number
    offset += 16 * 8;  // go past uuid
    offset += 64;      // go past streamid

    _ALIGN(offset, 32);

    return offset / 8;
}
uint64_t getPacketSize(uint8_t* traceBuf) {
    const size_t packetSizeOffset = getPacketSizeOffset();
    return *((uint64_t*)(&(traceBuf[packetSizeOffset]))) / 8;
}
bool isValidPacket(uint8_t* traceBuf) {
    // magic number is always at the beginning of the packet
    uint32_t magicNum = *((uint32_t*)(traceBuf));
    return magicNum == CTF_MAGIC_NUMBER;
}
void writeAll(uint8_t* buf, size_t size, std::string_view filePath) {
    FILE* outputFile = fopen(filePath.data(), "wb");
    if (outputFile == nullptr) { throw std::runtime_error("Failed to open file for writeAll"); }

    size_t byteWritten = 0;

    while (byteWritten < size) {
        const size_t byteLeft = size - byteWritten;
        byteWritten += fwrite(buf + byteWritten, sizeof(uint8_t), byteLeft, outputFile);

        if (ferror(outputFile)) { throw std::runtime_error("Error during write in writeAll"); }
    }
    fclose(outputFile);
}

void splitTrace(uint8_t* traceBuf, size_t traceBufSize, std::string splittedTracePath) {
    size_t totalPacket = 0;
    size_t totalSize   = 0;

    while (totalSize < traceBufSize && isValidPacket(traceBuf)) {
        uint64_t packetSize = getPacketSize(traceBuf);

        const std::string currentPacketOutputPath =
            splittedTracePath + "/stream_" + std::to_string(totalPacket);
        writeAll(traceBuf, packetSize, currentPacketOutputPath);

        ++totalPacket;
        traceBuf += packetSize;
        totalSize += packetSize;
    }

    std::cout << "Processed " << totalPacket << " packets" << std::endl;
}

void readTraceFile(const std::string_view traceFilePath,
                   uint8_t**              traceBuf,
                   size_t*                traceFileSize) {
    FILE* traceFile = fopen(traceFilePath.data(), "rb");
    if (traceFile == nullptr) { throw std::runtime_error("Failed to open trace file"); }

    fseek(traceFile, 0, SEEK_END);
    const long fileSize = ftell(traceFile);
    fseek(traceFile, 0, SEEK_SET);

    uint8_t* retBuf = new uint8_t[fileSize];
    if (retBuf == nullptr) { throw std::runtime_error("Failed to allocate trace file buf"); }
    if (1 != fread(retBuf, fileSize, 1, traceFile)) {
        throw std::runtime_error("Failed to read into trace file buf");
    }
    fclose(traceFile);

    *traceBuf      = retBuf;
    *traceFileSize = static_cast<size_t>(fileSize);
}
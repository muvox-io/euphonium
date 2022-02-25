#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(uint8_t *static_buffer, size_t dataCapacity)
{
    this->dataCapacity = dataCapacity;
    buffer = static_buffer;
};

size_t CircularBuffer::write(const uint8_t *data, size_t bytes)
{
    if (bytes == 0)
        return 0;

    std::lock_guard<std::mutex> guard(bufferMutex);
    size_t bytesToWrite = std::min(bytes, dataCapacity - dataSize);
    // Write in a single step
    if (bytesToWrite <= dataCapacity - endIndex)
    {
        memcpy(buffer + endIndex, data, bytesToWrite);
        endIndex += bytesToWrite;
        if (endIndex == dataCapacity)
            endIndex = 0;
    }
    // Write in two steps
    else
    {
        size_t firstChunkSize = dataCapacity - endIndex;
        memcpy(buffer + endIndex, data, firstChunkSize);
        size_t secondChunkSize = bytesToWrite - firstChunkSize;
        memcpy(buffer, data + firstChunkSize, secondChunkSize);
        endIndex = secondChunkSize;
    }

    dataSize += bytesToWrite;
    return bytesToWrite;
}

void CircularBuffer::emptyBuffer() {
    std::lock_guard<std::mutex> guard(bufferMutex);
    begIndex = 0;
    dataSize = 0;
    endIndex = 0;
}

size_t CircularBuffer::read(uint8_t *data, size_t bytes)
{
    if (bytes == 0)
        return 0;

    std::lock_guard<std::mutex> guard(bufferMutex);
    size_t bytesToRead = std::min(bytes, dataSize);

    // Read in a single step
    if (bytesToRead <= dataCapacity - begIndex)
    {
        memcpy(data, buffer + begIndex, bytesToRead);
        begIndex += bytesToRead;
        if (begIndex == dataCapacity)
            begIndex = 0;
    }
    // Read in two steps
    else
    {
        size_t firstChunkSize = dataCapacity - begIndex;
        memcpy(data, buffer + begIndex, firstChunkSize);
        size_t secondChunkSize = bytesToRead - firstChunkSize;
        memcpy(data + firstChunkSize, buffer, secondChunkSize);
        begIndex = secondChunkSize;
    }

    dataSize -= bytesToRead;
    return bytesToRead;
}

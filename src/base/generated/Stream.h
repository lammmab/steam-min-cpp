#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>

using byte = unsigned char;

class Stream
{
public:
    void Write(int32_t val) {}
    void Write(uint32_t val) {}
    void Write(int64_t val) {}
    void Write(uint64_t val) {}
    void Write(int16_t val) {}
    void Write(uint16_t val) {}
    void Write(unsigned char val) {}
    void Write(const std::vector<unsigned char>& val) {}

    template <typename T>
    T Read()
    {
        return T();
    }

    template <typename T>
    std::vector<T> ReadArray(size_t size)
    {
        return std::vector<T>(size);
    }

    // STUBBED
    void Read(byte* buffer, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
            buffer[i] = 0;
    }
};
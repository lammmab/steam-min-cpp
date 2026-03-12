// GPT
#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <cstring>

#include "utils/err.h"

using byte = unsigned char;

namespace Steam::Utils {
    class Stream
    {
    public:
        // Write-mode constructor (default)
        explicit Stream()
            : readBuffer(nullptr), position(0)
        {}

        // Read-mode constructor
        explicit Stream(const std::vector<byte>& input)
            : readBuffer(&input), buffer(), position(0)
        {}
        
        // Read-mode constructor with offset
        explicit Stream(const std::vector<byte>& input, size_t offset)
            : readBuffer(&input), buffer(), position(offset)
        {}

        // -----------------------
        // State
        // -----------------------

        size_t Position() const { return position; }

        size_t Length() const
        {
            return readBuffer ? readBuffer->size() : buffer.size();
        }

        const std::vector<byte>& GetBuffer() const
        {
            return buffer;
        }

        std::vector<byte> MoveBuffer()
        {
            return std::move(buffer);
        }

        // -----------------------
        // Writing (little-endian)
        // -----------------------

        template<typename T>
        void Write(T value)
        {
            static_assert(std::is_integral_v<T>, "Integral required");

            if (readBuffer)
                throw std::runtime_error("Stream is in read mode");

            for (size_t i = 0; i < sizeof(T); ++i)
                buffer.push_back(static_cast<byte>((value >> (8 * i)) & 0xFF));
        }

        void Write(const std::vector<byte>& data)
        {
            if (readBuffer)
                throw std::runtime_error("Stream is in read mode");

            buffer.insert(buffer.end(), data.begin(), data.end());
        }

        void Write(const byte* data, size_t size)
        {
            if (readBuffer)
                throw std::runtime_error("Stream is in read mode");

            buffer.insert(buffer.end(), data, data + size);
        }

        void Write(const void* data, size_t size)
        {
            const uint8_t* ptr = static_cast<const uint8_t*>(data);
            buffer.insert(buffer.end(), ptr, ptr + size);
        }

        // -----------------------
        // Reading (little-endian)
        // -----------------------

        template<typename T>
        T Read()
        {
            static_assert(std::is_integral_v<T>, "Integral required");

            if (!readBuffer)
                throw std::runtime_error("Stream is in write mode");

            if (position + sizeof(T) > readBuffer->size())
                throw std::runtime_error("Stream underflow");

            T value = 0;
            for (size_t i = 0; i < sizeof(T); ++i)
                value |= static_cast<T>((*readBuffer)[position + i]) << (8 * i);

            position += sizeof(T);
            return value;
        }

        void Read(byte* out, size_t size)
        {
            if (!readBuffer)
                throw std::runtime_error("Stream is in write mode");

            if (position + size > readBuffer->size())
                throw std::runtime_error("Stream underflow");

            std::memcpy(out, readBuffer->data() + position, size);
            position += size;
        }

        std::vector<byte> ReadBytes(size_t size)
        {
            if (!readBuffer)
                throw std::runtime_error("Stream is in write mode");

            if (position + size > readBuffer->size())
                throw std::runtime_error("Stream underflow");

            std::vector<byte> out(
                readBuffer->begin() + position,
                readBuffer->begin() + position + size
            );

            position += size;
            return out;
        }

        template<typename T>
        std::vector<T> ReadArray(size_t count)
        {
            static_assert(std::is_integral_v<T>, "Integral required");

            if (!readBuffer)
                throw std::runtime_error("Stream is in write mode");

            if (position + sizeof(T) * count > readBuffer->size())
                throw std::runtime_error("Stream underflow");

            std::vector<T> result;
            result.reserve(count);

            for (size_t i = 0; i < count; ++i)
                result.push_back(Read<T>());

            return result;
        }

    private:
        const std::vector<byte>* readBuffer;
        std::vector<byte> buffer;
        size_t position;
    };
}
// GPT
#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cstring>
#include <optional>

#include "base/generated/SteamLanguageInternal.hpp"

namespace Steam::Messaging::Messages {
    // Base message class for payload handling
    class MsgBase {
    protected:
        std::vector<uint8_t> payload;
        size_t readPos = 0;

    public:
        MsgBase(size_t reserve = 0) {
            payload.reserve(reserve);
        }

        // Payload access
        std::vector<uint8_t>& Payload() { return payload; }
        const std::vector<uint8_t>& Payload() const { return payload; }

        // Writing helpers
        void WriteByte(uint8_t b) { payload.push_back(b); }
        void WriteBytes(const std::vector<uint8_t>& data) {
            payload.insert(payload.end(), data.begin(), data.end());
        }
        template <typename T>
        void WriteValue(const T& value) {
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
            payload.insert(payload.end(), ptr, ptr + sizeof(T));
        }
        void WriteString(const std::string& str) {
            payload.insert(payload.end(), str.begin(), str.end());
        }
        void WriteNullTermString(const std::string& str) {
            WriteString(str);
            payload.push_back(0);
        }

        // Reading helpers
        uint8_t ReadByte() {
            if (readPos >= payload.size()) throw std::out_of_range("Read past end");
            return payload[readPos++];
        }
        template <typename T>
        T ReadValue() {
            if (readPos + sizeof(T) > payload.size()) throw std::out_of_range("Read past end");
            T val;
            std::memcpy(&val, payload.data() + readPos, sizeof(T));
            readPos += sizeof(T);
            return val;
        }
        std::string ReadNullTermString() {
            size_t start = readPos;
            while (readPos < payload.size() && payload[readPos] != 0) readPos++;
            std::string result(payload.begin() + start, payload.begin() + readPos);
            if (readPos < payload.size()) readPos++; // skip null
            return result;
        }

        void Seek(size_t pos) { readPos = pos; }
        size_t Tell() const { return readPos; }
    };

    // Templated message class with header
    template <typename THeader>
    class MsgBaseHdr : public MsgBase {
    public:
        THeader Header;

        virtual bool IsProto() const = 0;
        virtual Steam::Internal::Enums::EMsg MsgType() const = 0;

        std::optional<SteamID> steam_id;
        int SessionID = 0;
        uint64_t targetJobID = 0;
        uint64_t sourceJobID = 0;

        MsgBaseHdr(size_t reserve = 0) : MsgBase(reserve), Header() {}

        virtual std::vector<uint8_t> Serialize() const {
            Stream stream(StreamingMode::Write);
            Header.Serialize(stream);
            stream.Write(payload);
            return stream.MoveBuffer();
        }

        virtual ~MsgBaseHdr() = default;
    };
}
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#include "base/generated/SteamLanguage.h"

namespace MsgUtil
{
    static constexpr uint32_t ProtoMask = 0x80000000;

    inline uint32_t MakeMsg(SteamInternal::EMsg msg, bool isProto)
    {
        uint32_t value = static_cast<uint32_t>(msg);
        return isProto ? (value | ProtoMask) : value;
    }

    inline SteamInternal::EMsg GetMsg(uint32_t raw)
    {
        return static_cast<SteamInternal::EMsg>(raw & ~ProtoMask);
    }

    inline uint32_t MakeGCMsg(uint32_t msg, bool isProto)
    {
        return isProto ? (msg | ProtoMask) : msg;
    }

    inline uint32_t GetGCMsg(uint32_t raw)
    {
        return raw & ~ProtoMask;
    }
}

struct SteamID
{
private:
    uint64_t value;

public:
    SteamID() : value(0) {}
    explicit SteamID(uint64_t v) : value(v) {}

    uint64_t ConvertToUInt64() const
    {
        return value;
    }
};

struct GameID
{
private:
    uint64_t value;

public:
    GameID() : value(0) {}
    explicit GameID(uint64_t v) : value(v) {}

    uint64_t ConvertToUInt64() const
    {
        return value;
    }
};

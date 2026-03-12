// GPT
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#include "base/generated/SteamLanguage.hpp"

namespace Steam::MsgUtil
{
    static constexpr uint32_t PROTO_MASK = 0x80000000;

    inline uint32_t make_msg(Steam::Internal::Enums::EMsg msg, bool isProto)
    {
        uint32_t value = static_cast<uint32_t>(msg);
        return isProto ? (value | PROTO_MASK) : value;
    }

    inline Steam::Internal::Enums::EMsg get_msg(uint32_t raw)
    {
        return static_cast<Steam::Internal::Enums::EMsg>(raw & ~PROTO_MASK);
    }

    inline uint32_t make_gc_msg(uint32_t msg, bool isProto)
    {
        return isProto ? (msg | PROTO_MASK) : msg;
    }

    inline uint32_t get_gc_msg(uint32_t raw)
    {
        return raw & ~PROTO_MASK;
    }

    inline static bool is_protobuf_msg(uint32_t emsg)
    {
        return (emsg & PROTO_MASK) > 0;
    }
}

struct SteamID
{
private:
    uint64_t value = 0;

    static constexpr int ACCOUNT_ID_SHIFT   = 0;
    static constexpr int INSTANCE_SHIFT     = 32;
    static constexpr int ACCOUNT_TYPE_SHIFT = 52;
    static constexpr int UNIVERSE_SHIFT     = 56;

    static constexpr uint64_t ACCOUNT_ID_MASK   = 0xFFFFFFFFULL;
    static constexpr uint64_t INSTANCE_MASK     = 0xFFFFFULL;
    static constexpr uint64_t ACCOUNT_TYPE_MASK = 0xFULL;
    static constexpr uint64_t UNIVERSE_MASK     = 0xFFULL;

public:
    SteamID() = default;

    SteamID(
        uint32_t accountID,
        uint32_t instance,
        Steam::Internal::Enums::EUniverse universe,
        Steam::Internal::Enums::EAccountType accountType)
    {
        value =
            ((uint64_t)accountID << ACCOUNT_ID_SHIFT) |
            ((uint64_t)instance << INSTANCE_SHIFT) |
            ((uint64_t)accountType << ACCOUNT_TYPE_SHIFT) |
            ((uint64_t)universe << UNIVERSE_SHIFT);
    }

    SteamID(uint64_t val) {
        value = val;
    }

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

/// @internal
// GPT
#pragma once
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <steamclient/types/generated/SteamLanguage.hpp>
#include <string>
#include <vector>

namespace Steam::Utils::MsgUtil {
static constexpr uint32_t PROTO_MASK = 0x80000000;

inline uint32_t make_msg(Steam::Internal::Enums::EMsg msg, bool isProto) {
  uint32_t value = static_cast<uint32_t>(msg);
  return isProto ? (value | PROTO_MASK) : value;
}

inline Steam::Internal::Enums::EMsg get_msg(uint32_t raw) {
  return static_cast<Steam::Internal::Enums::EMsg>(raw & ~PROTO_MASK);
}

inline uint32_t make_gc_msg(uint32_t msg, bool isProto) {
  return isProto ? (msg | PROTO_MASK) : msg;
}

inline uint32_t get_gc_msg(uint32_t raw) { return raw & ~PROTO_MASK; }

inline static bool is_protobuf_msg(uint32_t emsg) {
  return (emsg & PROTO_MASK) > 0;
}
}  // namespace Steam::Utils::MsgUtil
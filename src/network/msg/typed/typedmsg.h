#pragma once

#include "network/msg/msg.h"

inline uint32_t read_le32(const uint8_t* ptr) {
    return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

template<typename T>
struct TypedMsg : public Msg {
    T body;
    TypedMsg<T>(const Msg& raw);
};

struct MsgChannelEncryptRequest {
    uint32_t ProtocolVersion;
    uint32_t Universe;
    std::vector<uint8_t> Challenge;

    size_t HEADER_SIZE = 16;

    void deserialize(const std::vector<uint8_t>& payload) {
        if (payload.size() < HEADER_SIZE + 8)
            throw std::runtime_error("Payload too short");

        ProtocolVersion = read_le32(payload.data() + HEADER_SIZE);
        Universe        = read_le32(payload.data() + HEADER_SIZE + 4);

        Challenge.assign(payload.begin() + HEADER_SIZE + 8, payload.end());
    }
};

template<typename T>
TypedMsg<T>::TypedMsg(const Msg& raw) {
    emsg = raw.emsg;
    payload = raw.payload;
    body.deserialize(payload);
}
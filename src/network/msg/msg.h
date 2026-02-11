#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <memory>
#include <google/protobuf/message.h>
#include "protogen/steammessages_base.pb.h"
#include "protogen/enums_clientserver.pb.h"
#include <iostream>
#include <sstream>
#include <iomanip>

inline bool is_protobuf_emsg(uint32_t emsg) {
    return emsg != k_EMsgChannelEncryptRequest &&
           emsg != k_EMsgChannelEncryptResponse &&
           emsg != k_EMsgChannelEncryptResult;
}

struct Msg {
    uint32_t emsg;
    std::vector<uint8_t> payload;

    void parse(const std::vector<uint8_t>& data) {
        if (data.size() < 4)
            throw std::runtime_error("Msg too short");

        std::memcpy(&emsg, data.data(), 4);
        emsg &= ~0x80000000;

        payload.assign(data.begin() + 4, data.end());
    }

    std::string to_string(size_t max_payload_bytes = 64) const {
        std::ostringstream oss;
        oss << "Msg emsg: " << emsg << "\n";
        oss << "Payload (hex, first " << max_payload_bytes << " bytes): ";
        size_t len = std::min(payload.size(), max_payload_bytes);
        for (size_t i = 0; i < len; ++i)
            oss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(payload[i]) << " ";
        if (payload.size() > max_payload_bytes) oss << "...";
        oss << "\n";
        return oss.str();
    }
};
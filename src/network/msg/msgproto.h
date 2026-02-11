#include <cstdint>
#include <vector>
#include "protogen/steammessages_base.pb.h"
#include <iostream>
#include <iomanip> // for std::hex
#include <sstream>

struct MsgProto {
    uint32_t emsg;
    CMsgProtoBufHeader header;
    std::vector<uint8_t> payload;

    void parse(const std::vector<uint8_t>& data) {
        if (data.size() < 8)
            throw std::runtime_error("Proto msg too short");

        uint32_t raw_emsg;
        uint32_t header_len;

        std::memcpy(&raw_emsg, data.data(), 4);
        std::memcpy(&header_len, data.data() + 4, 4);

        emsg = raw_emsg & ~0x80000000;

        size_t offset = 8;
        header.ParseFromArray(data.data() + offset, header_len);
        offset += header_len;

        payload.assign(data.begin() + offset, data.end());
    }

    std::string to_string(size_t max_payload_bytes = 64) const {
        std::ostringstream oss;

        oss << "MsgProto emsg: " << emsg << "\n";

        oss << "Header:\n" << header.DebugString();

        oss << "Payload (hex, first " << max_payload_bytes << " bytes): ";
        size_t display_len = std::min(payload.size(), max_payload_bytes);
        for (size_t i = 0; i < display_len; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(payload[i]) << " ";
        }
        if (payload.size() > max_payload_bytes) oss << "...";
        oss << "\n";

        oss << "Payload (ascii, first " << max_payload_bytes << " bytes): ";
        for (size_t i = 0; i < display_len; ++i) {
            char c = static_cast<char>(payload[i]);
            if (std::isprint(static_cast<unsigned char>(c)))
                oss << c;
            else
                oss << '.';
        }
        if (payload.size() > max_payload_bytes) oss << "...";
        oss << "\n";

        return oss.str();
    }
};
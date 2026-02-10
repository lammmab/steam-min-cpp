#include <cstdint>
#include <vector>
#include "protogen/steammessages_base.pb.h"

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
};
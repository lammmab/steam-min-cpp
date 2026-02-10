#include <cstdint>
#include <vector>
#include <memory.h>
#include <stdexcept>

struct ExtendedMsgHdr {
    uint32_t msg;
    uint8_t headerSize;
    uint8_t headerVersion;
    int64_t targetJobID;
    int64_t sourceJobID;
    uint8_t headerCanary;
    int64_t steamID;
    int32_t sessionID;

    static constexpr size_t SIZE = 36;

    void parse(const std::vector<uint8_t>& data) {
        if (data.size() < SIZE)
            throw std::runtime_error("Data too short for header");

        std::memcpy(&msg, data.data(), sizeof(msg));
        headerSize = data[4];
        headerVersion = data[5];
        std::memcpy(&targetJobID, data.data() + 6, sizeof(targetJobID));
        std::memcpy(&sourceJobID, data.data() + 14, sizeof(sourceJobID));
        headerCanary = data[22];
        std::memcpy(&steamID, data.data() + 23, sizeof(steamID));
        std::memcpy(&sessionID, data.data() + 31, sizeof(sessionID));

        if (headerSize != SIZE || headerVersion != 2)
            throw std::runtime_error("Invalid header");
    }
};

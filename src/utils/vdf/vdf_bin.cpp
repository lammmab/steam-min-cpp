#include "vdf.hpp"
#include <stdexcept>
#include <cstring>

namespace Steam::Utils::VDF {

    static std::string read_cstring(const uint8_t* data, size_t size, size_t& pos) {
        std::string s;
        while (pos < size && data[pos] != 0)
            s.push_back((char)data[pos++]);
        if (pos >= size)
            throw std::runtime_error("Unexpected end of buffer while reading cstring");
        pos++;
        return s;
    }

    static int32_t read_int32(const uint8_t* data, size_t size, size_t& pos) {
        if (pos + sizeof(int32_t) > size)
            throw std::runtime_error("Binary VDF overflow (int32)");
        int32_t v;
        std::memcpy(&v, data + pos, sizeof(v));
        pos += sizeof(v);
        return v;
    }

    static uint64_t read_uint64(const uint8_t* data, size_t size, size_t& pos) {
        if (pos + sizeof(uint64_t) > size)
            throw std::runtime_error("Binary VDF overflow (uint64)");
        uint64_t v;
        std::memcpy(&v, data + pos, sizeof(v));
        pos += sizeof(v);
        return v;
    }

    static float read_float(const uint8_t* data, size_t size, size_t& pos) {
        if (pos + sizeof(float) > size)
            throw std::runtime_error("Binary VDF overflow (float)");
        float v;
        std::memcpy(&v, data + pos, sizeof(v));
        pos += sizeof(v);
        return v;
    }

    static Object parse_object(const uint8_t* data, size_t size, size_t& pos) {
        Object obj;
        while (pos < size)
        {
            uint8_t type = data[pos++];
            if (type == 0x08) // BIN_END
                break;
            std::string key = read_cstring(data, size, pos);
            switch (type)
            {
                case 0x00: // nested object
                    obj[key].value = parse_object(data, size, pos);
                    break;
                case 0x01: // string
                    obj[key].value = read_cstring(data, size, pos);
                    break;
                case 0x02: // int32
                    obj[key].value = read_int32(data, size, pos);
                    break;
                case 0x03: // float
                    obj[key].value = read_float(data, size, pos);
                    break;
                case 0x07: // uint64
                    obj[key].value = read_uint64(data, size, pos);
                    break;
                default:
                    throw std::runtime_error("Unknown binary VDF type");
            }
        }
        return obj;
    }

    Object parse_binary(const uint8_t* data, size_t size) {
        size_t pos = 0;
        return parse_object(data, size, pos);
    }

}
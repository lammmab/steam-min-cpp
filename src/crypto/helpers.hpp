#pragma once
#include <vector>
#include <cstdint>

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/crc.h>

namespace Steam::Crypto::Helpers {
    inline static std::vector<uint8_t> crc_to_vector(uint32_t crc)
    {
        std::vector<uint8_t> out = {
            static_cast<uint8_t>(crc & 0xFF),
            static_cast<uint8_t>((crc >> 8) & 0xFF),
            static_cast<uint8_t>((crc >> 16) & 0xFF),
            static_cast<uint8_t>((crc >> 24) & 0xFF)
        };
        return out;
    }

    inline std::vector<uint8_t> generate_random_bytes(size_t length,CryptoPP::AutoSeededRandomPool& rng)
    {
        std::vector<uint8_t> buffer(length);
        rng.GenerateBlock(buffer.data(), buffer.size());
        return buffer;
    }

    std::vector<uint8_t> rsa_encrypt_oaep_sha1(
        const std::vector<uint8_t>& pubKeyDer,
        const std::vector<uint8_t>& plaintext,
        CryptoPP::AutoSeededRandomPool& rng
    );
    uint32_t crc32_hash(const std::vector<uint8_t>& data);

}
#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/crc.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

namespace Steam::Crypto::Helpers {
    inline constexpr static size_t IV_LENGTH = 16;
    inline constexpr static size_t IV_RAND_LENGTH = 3;
    inline constexpr static size_t BLOCK_SIZE = 16;
    inline constexpr static size_t HASH_LENGTH = IV_LENGTH - IV_RAND_LENGTH;

    inline static std::vector<uint8_t> crc_to_vector(uint32_t crc)
    {
        return std::vector<uint8_t>{
            static_cast<uint8_t>(crc & 0xFF),
            static_cast<uint8_t>((crc >> 8) & 0xFF),
            static_cast<uint8_t>((crc >> 16) & 0xFF),
            static_cast<uint8_t>((crc >> 24) & 0xFF)
        };
    }

    inline std::vector<uint8_t> generate_random_bytes(
        size_t length,
        CryptoPP::AutoSeededRandomPool& rng
    )
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
    
    std::vector<uint8_t> symmetric_encrypt_hmac_iv(
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& session_key,
        const std::vector<uint8_t>& hmac_secret,
        CryptoPP::RandomNumberGenerator& rng
    );

    std::vector<uint8_t> symmetric_decrypt_hmac_iv(
        const std::vector<uint8_t>& input,
        const std::vector<uint8_t>& session_key,
        const std::vector<uint8_t>& hmac_secret
    );

    void generate_iv(
        const std::vector<uint8_t>& plaintext,
        std::vector<uint8_t>& iv,
        const std::vector<uint8_t>& hmac_secret,
        CryptoPP::RandomNumberGenerator& rng
    );

    bool validate_iv(
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& iv,
        const std::vector<uint8_t>& hmac_secret
    );

    inline size_t const calculate_max_encrypted_length(size_t plaintext_len) {
        return IV_LENGTH + (((plaintext_len + BLOCK_SIZE) / BLOCK_SIZE) * BLOCK_SIZE);
    }

}
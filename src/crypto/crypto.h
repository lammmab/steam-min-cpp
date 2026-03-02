#pragma once

#include <vector>
#include <utility>
#include <stdexcept>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <cryptopp/rsa.h>
#include <cryptopp/oaep.h>
#include <cryptopp/sha.h>
#include <cryptopp/hmac.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/crc.h>
#include <sstream>

class SteamCrypto {
public:
    struct SessionKey {
        std::vector<uint8_t> aes_key;
        std::vector<uint8_t> encrypted_key;
        std::vector<uint8_t> hmac;
    };

    SteamCrypto();

    SessionKey generate_session_key(
        const std::vector<uint8_t>& hmac_secret = {}
    );

    std::vector<uint8_t> symmetric_encrypt(
        const std::vector<uint8_t>& message,
        const std::vector<uint8_t>& key
    );

    std::vector<uint8_t> symmetric_encrypt_HMAC(
        const std::vector<uint8_t>& message,
        const std::vector<uint8_t>& key,
        const std::vector<uint8_t>& hmac_secret
    );

    std::vector<uint8_t> symmetric_decrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& key
    );

    std::vector<uint8_t> symmetric_decrypt_HMAC(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& key,
        const std::vector<uint8_t>& hmac_secret
    );
    std::vector<uint8_t> generate_random_bytes(size_t len);
    CryptoPP::RSA::PublicKey get_universe_pub() { return universe_pub_; }

    std::vector<uint8_t> hmac_sha1(
        const std::vector<uint8_t>& secret,
        const std::vector<uint8_t>& data
    );
    std::vector<uint8_t> encrypt_RSA_OAEP_SHA1(
        const std::vector<uint8_t>& data
    );
    CryptoPP::AutoSeededRandomPool rng;

    inline uint32_t crc32(const std::vector<uint8_t>& data) {
        CryptoPP::CRC32 crc;
        crc.Update(data.data(), data.size());

        CryptoPP::SecByteBlock hash(crc.DigestSize());
        crc.TruncatedFinal(hash, hash.size());

        uint32_t result = 0;
        for (size_t i = 0; i < 4; ++i) {
            result |= static_cast<uint32_t>(hash[i]) << (8 * i);
        }
        return result;
    }
    
private:
    CryptoPP::RSA::PublicKey universe_pub_;

    void load_universe_pub();



    std::vector<uint8_t> pkcs7_pad(const std::vector<uint8_t>& data, size_t block_size);
    std::vector<uint8_t> pkcs7_unpad(const std::vector<uint8_t>& data);
};
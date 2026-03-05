#pragma once

#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>

#include "base/generated/SteamLanguageInternal.h"
#include "base/packetbase.h"
#include "base/clientmsg.h"

#include "utils/err.h"

inline static uint32_t crc32_hash(const std::vector<uint8_t>& data)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint8_t b : data)
    {
        crc ^= b;
        for (int i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }

    return crc ^ 0xFFFFFFFF;
};

inline static std::vector<uint8_t> crc_to_little_endian(uint32_t crc)
{
    return std::vector<uint8_t>{
        static_cast<uint8_t>(crc & 0xFF),
        static_cast<uint8_t>((crc >> 8) & 0xFF),
        static_cast<uint8_t>((crc >> 16) & 0xFF),
        static_cast<uint8_t>((crc >> 24) & 0xFF)
    };
};

constexpr uint8_t STEAM_PUBLIC_KEY_DER[] = {
0x30, 0x81, 0x9D, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01,
0x05, 0x00, 0x03, 0x81, 0x8B, 0x00, 0x30, 0x81, 0x87, 0x02, 0x81, 0x81, 0x00, 0xDF, 0xEC, 0x1A,
0xD6, 0x2C, 0x10, 0x66, 0x2C, 0x17, 0x35, 0x3A, 0x14, 0xB0, 0x7C, 0x59, 0x11, 0x7F, 0x9D, 0xD3,
0xD8, 0x2B, 0x7A, 0xE3, 0xE0, 0x15, 0xCD, 0x19, 0x1E, 0x46, 0xE8, 0x7B, 0x87, 0x74, 0xA2, 0x18,
0x46, 0x31, 0xA9, 0x03, 0x14, 0x79, 0x82, 0x8E, 0xE9, 0x45, 0xA2, 0x49, 0x12, 0xA9, 0x23, 0x68,
0x73, 0x89, 0xCF, 0x69, 0xA1, 0xB1, 0x61, 0x46, 0xBD, 0xC1, 0xBE, 0xBF, 0xD6, 0x01, 0x1B, 0xD8,
0x81, 0xD4, 0xDC, 0x90, 0xFB, 0xFE, 0x4F, 0x52, 0x73, 0x66, 0xCB, 0x95, 0x70, 0xD7, 0xC5, 0x8E,
0xBA, 0x1C, 0x7A, 0x33, 0x75, 0xA1, 0x62, 0x34, 0x46, 0xBB, 0x60, 0xB7, 0x80, 0x68, 0xFA, 0x13,
0xA7, 0x7A, 0x8A, 0x37, 0x4B, 0x9E, 0xC6, 0xF4, 0x5D, 0x5F, 0x3A, 0x99, 0xF9, 0x9E, 0xC4, 0x3A,
0xE9, 0x63, 0xA2, 0xBB, 0x88, 0x19, 0x28, 0xE0, 0xE7, 0x14, 0xC0, 0x42, 0x89, 0x02, 0x01, 0x11
};

inline static std::vector<uint8_t> getSteamPublicKey() {
    return std::vector<uint8_t>(
        STEAM_PUBLIC_KEY_DER,
        STEAM_PUBLIC_KEY_DER + sizeof(STEAM_PUBLIC_KEY_DER)
    );
}

inline static std::vector<uint8_t> rsa_oaep_sha1_encrypt(
    const std::vector<uint8_t>& publicKeyDer,
    const std::vector<uint8_t>& plaintext)
{
    const unsigned char* p = publicKeyDer.data();
    EVP_PKEY* pkey = d2i_PUBKEY(NULL, &p, (int)publicKeyDer.size());
    if (!pkey) {
        throw std::runtime_error("Failed to parse DER public key");
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx) {
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to create EVP_PKEY_CTX");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("EVP_PKEY_encrypt_init failed");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to set OAEP padding");
    }

    size_t outlen = 0;
    if (EVP_PKEY_encrypt(ctx, NULL, &outlen,
                         plaintext.data(), plaintext.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to determine output length");
    }

    std::vector<uint8_t> outbuf(outlen);

    if (EVP_PKEY_encrypt(ctx, outbuf.data(), &outlen,
                         plaintext.data(), plaintext.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("RSA OAEP encryption failed");
    }

    outbuf.resize(outlen);

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return outbuf;
}

inline std::vector<uint8_t> generate_random_bytes(size_t length) {
    std::vector<uint8_t> buffer(length);
    if (RAND_bytes(buffer.data(), static_cast<int>(length)) != 1) {
        throw std::runtime_error("OpenSSL RAND_bytes failed");
    }
    return buffer;
}

struct EncryptionResponse {
    Msg<SteamInternal::Internal::MsgChannelEncryptResponse> msg;
    std::vector<uint8_t> key;
};

// 1. Grab the challenge (usually 16 bytes) std::vector<uint8_t>
// 2. Create an empty MsgChannelEncryptResponse (a legacy, non-protobuf Msg.), with the NORMAL MsgHdr
// 3. Generate 32 random bytes, this is the AES key
// 4. Create a vector array that contains FIRST the AES key, and SECOND the challenge from request.Body.Challenge ([AES KEY][RANDOM CHALLENGE]), 48 bytes expected
// 5. Using the Universe Public Key, encrypt this vector array with OAEP SHA1 padding
// 6. Generate the CRC32 hash of the encrypted vector array
// 7. Make a final array / append to the response payload in THIS ORDER:
// 8. the encrypted vector array, the crc32 hash key, 4 nil bytes of padding

// https://github.com/SteamRE/SteamKit/blob/master/SteamKit2/SteamKit2/Networking/Steam3/EnvelopeEncryptedConnection.cs#L131
inline Msg<SteamInternal::Internal::MsgChannelEncryptResponse> generate_encryption_response(const PacketMsg& packet) {
    Msg<SteamInternal::Internal::MsgChannelEncryptRequest> request(packet);
    std::vector<uint8_t> challenge = request.Payload();
    std::vector<uint8_t> aes_key = generate_random_bytes(32);

    RUNTIME_ASSERT(request.Body.protocolVersion == 1, "Encryption handshake protocol version mismatch!");
    RUNTIME_ASSERTF(request.Body.universe == SteamInternal::EUniverse::Public, "Expected public universe, but got {}",static_cast<int>(request.Body.universe));
    RUNTIME_ASSERTF(challenge.size() >= 16, "Expected 16+ byte handshake, but got {} bytes",static_cast<int>(challenge.size()));

    std::vector<uint8_t> handshake = aes_key;
    handshake.insert(
        handshake.end(),
        challenge.begin(),
        challenge.end()
    ); // 48 bytes
    
    std::vector<uint8_t> encrypted_key_vector = rsa_oaep_sha1_encrypt(getSteamPublicKey(),handshake);
    std::vector<uint8_t> hash_bytes = crc_to_little_endian(crc32_hash(encrypted_key_vector));

    Msg<SteamInternal::Internal::MsgChannelEncryptResponse> response;
    std::vector<uint8_t> payload = encrypted_key_vector;
    payload.insert(
        payload.end(),
        hash_bytes.begin(),
        hash_bytes.end()
    );
    payload.insert(payload.end(), {0,0,0,0});
    spdlog::info("Writing {} bytes to EncryptionResponse",payload.size());
    response.WriteBytes(payload);

    return response;
}
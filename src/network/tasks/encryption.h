#pragma once

#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
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

inline static std::vector<uint8_t> rsa_oaep_sha1_encrypt_little_endian(const std::vector<uint8_t>& publicKeyDer, 
                                         const std::vector<uint8_t>& plaintext) 
{
    // Load RSA public key from DER bytes
    const unsigned char* ptr = publicKeyDer.data();
    RSA* rsa = d2i_RSA_PUBKEY(nullptr, &ptr, static_cast<long>(publicKeyDer.size()));
    if (!rsa) throw std::runtime_error("Failed to load RSA public key");

    std::vector<uint8_t> ciphertext(RSA_size(rsa));
    int result = RSA_public_encrypt(
        static_cast<int>(plaintext.size()),
        plaintext.data(),
        ciphertext.data(),
        rsa,
        RSA_PKCS1_OAEP_PADDING // OAEP uses SHA1 by default in OpenSSL < 3.0
    );

    RSA_free(rsa);

    if (result == -1) {
        unsigned long err = ERR_get_error();
        throw std::runtime_error("RSA_public_encrypt failed: " + std::string(ERR_error_string(err, nullptr)));
    }

    // Convert to little-endian to match C# byte order
    std::reverse(ciphertext.begin(), ciphertext.begin() + result);
    ciphertext.resize(result); // actual size may be less than RSA_size
    return ciphertext;
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

inline EncryptionResponse generate_encryption_response(const PacketClientMsg& packet) {
    Msg<SteamInternal::Internal::MsgChannelEncryptRequest> request(packet);
    std::vector<uint8_t> challenge = request.Payload();
    std::vector<uint8_t> aes_key = generate_random_bytes(32);

    RUNTIME_ASSERT(request.Body.protocolVersion == 1, "Encryption handshake protocol version mismatch!");
    RUNTIME_ASSERTF(request.Body.universe == SteamInternal::EUniverse::Public, "Expected public universe, but got {}",static_cast<int>(request.Body.universe));
    RUNTIME_ASSERT(challenge.size() == 16, "Encryption handshake must be 16 bytes!");

    std::vector<uint8_t> handshake = aes_key;
    handshake.insert(
        handshake.end(),
        challenge.begin(),
        challenge.end()
    ); // 48 bytes
    
    std::vector<uint8_t> encrypted_key_vector = rsa_oaep_sha1_encrypt_little_endian(getSteamPublicKey(),handshake);
    std::vector<uint8_t> hash_bytes = crc_to_little_endian(crc32_hash(encrypted_key_vector));

    Msg<SteamInternal::Internal::MsgChannelEncryptResponse> response;
    std::vector<uint8_t> payload = encrypted_key_vector;
    payload.insert(
        payload.end(),
        hash_bytes.begin(),
        hash_bytes.end()
    );
    payload.insert(payload.end(), {0,0,0,0});
    response.WriteBytes(payload);

    return EncryptionResponse{response,aes_key};
}
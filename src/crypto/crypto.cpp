#include "crypto/crypto.hpp"

using namespace Steam::Messaging;

// Generates the proper MsgChannelEncryptResponse with RSA OAEP SHA1 and CRC32
ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResponse> 
    Steam::Crypto::EncryptionManager::generate_encryption_response(
        const Packets::PacketMsg& packet
) {
    ClientMessages::Msg<Steam::Internal::MsgChannelEncryptRequest> request(packet);
    std::vector<uint8_t> payload = request.Payload();

    std::vector<uint8_t> challenge(
        payload.begin() + 8,
        payload.end()
    ); // skip the 8 byte protocolVersion and universe

    RUNTIME_ASSERT(request.Body.protocolVersion == 1, "Encryption handshake protocol version mismatch!");
    RUNTIME_ASSERTF(request.Body.universe == Steam::Internal::Enums::EUniverse::Public, "Expected public universe, but got %d",static_cast<int>(request.Body.universe));
    RUNTIME_ASSERTF(challenge.size() == 16, "Expected 16 byte handshake, but got %d bytes",static_cast<int>(challenge.size()));

    std::vector<uint8_t> handshake = session_key_;
    handshake.insert(
        handshake.end(),
        challenge.begin(),
        challenge.end()
    ); // 48 bytes
    
    std::vector<uint8_t> encrypted_key_vector = Steam::Crypto::Helpers::rsa_encrypt_oaep_sha1(getSteamPublicKey(),handshake,rng_);
    uint32_t hash_bytes = Steam::Crypto::Helpers::crc32_hash(encrypted_key_vector);
    
    ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResponse> response;
    
    response.WriteBytes(encrypted_key_vector);
    response.WriteBytes(Steam::Crypto::Helpers::crc_to_vector(hash_bytes));
    response.WriteBytes({0,0,0,0});

    return response;
}


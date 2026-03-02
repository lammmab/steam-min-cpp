#pragma once

#include "crypto/crypto.h"
#include "network/msg/msg.h"
#include "network/msg/typed/typedmsg.h"
#include <protogen/enums_clientserver.pb.h>

class SteamEncryptionHandler {
public:
    inline Msg generate_encryption_response(const TypedMsg<MsgChannelEncryptRequest>& request) {
        key_ = crypto_.generate_random_bytes(32);
        std::vector<uint8_t> handshake_blob = key_;

        handshake_blob.insert(
            handshake_blob.end(),
            request.body.Challenge.begin(),
            request.body.Challenge.end()
        );
        std::vector<uint8_t> encrypted_blob = crypto_.encrypt_RSA_OAEP_SHA1(handshake_blob);
        uint32_t crc = crypto_.crc32(encrypted_blob);

        Msg m;
        m.emsg = EMsg::k_EMsgChannelEncryptResponse;
        m.payload.insert(m.payload.end(), encrypted_blob.begin(), encrypted_blob.end());

        m.payload.insert(
            m.payload.end(),
            reinterpret_cast<uint8_t*>(&crc),
            reinterpret_cast<uint8_t*>(&crc) + sizeof(crc)
        );

        m.payload.insert(m.payload.end(), {0,0,0,0});
        return m;
    }

private:
    std::vector<uint8_t> key_;
    SteamCrypto crypto_;
};
#include "network/cmclient.hpp"
#include <iostream>
#include "utils/macros.h"
#include "protogen/steammessages_clientserver_login.pb.h"
#include "base/generated/SteamUtils.hpp"
#include "utils/gzip/gzip_helpers.hpp"
#include <boost/endian/conversion.hpp>

FILE_LOGGER();

using namespace Steam::Messaging;
using GZip = Steam::Utils::GZip;

// Connect via TCP, then start message loop
void CMClient::start_session() {
    try {
        connection_->set_on_frame([this](const std::vector<uint8_t>& frame) {
            consume_frame(frame);
        });

        connection_->network_connect();
    } catch (const std::exception& e) {
        logger->error("Could not connect via TCP: {}",e.what());
    }
}

// Start consuming TCP messages
void CMClient::consume_frame(const std::vector<uint8_t>& frame, bool encrypt) {
    if (frame.size() < 4) return;
    std::vector<uint8_t> data = frame;

    if (encrypt && channel_secured_) {
        data = crypto_.process_incoming_encrypted_message(frame);
    }

    uint32_t emsg = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    try {
        if (is_encryption_msg(emsg)) {
            // We're a raw Msg
            auto raw = std::make_unique<Packets::PacketMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            Steam::Dispatch::dispatch_msg(*this,*raw);
        } else if (MsgUtil::is_protobuf_msg(emsg)) {
            // We're a protobuf Msg
            auto proto = std::make_unique<Packets::PacketClientMsgProtobuf>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            Steam::Dispatch::dispatch_proto(*this, *proto);
        } else {
            // We're a struct msg
            /*logger->info("Received struct Msg");
            auto str   = std::make_unique<Packets::PacketClientMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            emit(*str);*/
        }

    } catch (const std::exception& e) {
        logger->error("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

void CMClient::send_msg_impl(std::vector<byte> buffer)
{
    if (channel_secured_) {
        buffer = crypto_.process_outgoing_encrypted_message(buffer);
    }
    logger->info("Sending buffer of length {}", buffer.size());
    connection_->async_send(buffer);
}

void CMClient::shutdown() {
    if (connection_ && connection_->is_connected())
        connection_->network_close();
    
    logger->info("Closed connection and CMClient successfully");
}
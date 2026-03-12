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

        setup_handlers();

        connection_->network_connect();
        logger->info("Connected successfully.");
    } catch (const std::exception& e) {
        logger->error("Could not connect via TCP: {}",e.what());
    }
}

// Start consuming TCP messages
void CMClient::consume_frame(const std::vector<uint8_t>& frame, bool encrypt) {
    logger->info("Received frame of size {}", frame.size());
    if (frame.size() < 4) return;
    std::vector<uint8_t> data = frame;

    if (encrypt && channel_secured_) {
        data = crypto_.process_incoming_encrypted_message(frame);
    }

    uint32_t emsg = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    try {
        if (is_encryption_msg(emsg)) {
            // We're a raw Msg
            logger->info("Received raw Msg");
            auto raw = std::make_unique<Packets::PacketMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            Steam::Dispatch::dispatch_msg(*this,*raw);
        } else if (MsgUtil::is_protobuf_msg(emsg)) {
            // We're a protobuf Msg
            logger->info("Received proto Msg");
            auto proto = std::make_unique<Packets::PacketClientMsgProtobuf>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            Steam::Dispatch::dispatch_proto(*this, *proto);
            //emit(*proto);
        } else {
            // We're a struct msg
            logger->info("Received struct Msg");
            auto str   = std::make_unique<Packets::PacketClientMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            emit(*str);
        }

    } catch (const std::exception& e) {
        logger->error("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

void CMClient::setup_handlers() {
    on<Packets::PacketClientMsgProtobuf>([this](const Packets::PacketClientMsgProtobuf& msg) {
        rcv_msg_proto(msg);
    });
}
/*

bool read_frame(
    const std::vector<uint8_t>& payload,
    size_t& offset,
    std::vector<uint8_t>& out)
{
    if (offset + sizeof(uint32_t) > payload.size())
        return false;

    uint32_t subSize;
    memcpy(&subSize, payload.data() + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    subSize = boost::endian::native_to_little(subSize);

    if (offset + subSize > payload.size())
        return false;

    out.assign(
        payload.begin() + offset,
        payload.begin() + offset + subSize
    );

    offset += subSize;
    return true;
}

void CMClient::parse_multi(const Packets::PacketClientMsgProtobuf& msg) {
    ClientMessages::MsgProto<CMsgMulti> multi(msg);
    std::vector<uint8_t> payload(multi.Body.message_body().begin(), multi.Body.message_body().end());
    size_t size = multi.Body.size_unzipped();
    if (size > 0) payload = GZip::gzip_decompress(payload,size);
    size_t offset = 0;
    std::vector<uint8_t> frame;

    while (read_frame(payload, offset, frame))
    {
        consume_frame(frame,false); // Override the default encryption as multi msgs are decrypted at this point
    }

}
*/
void CMClient::rcv_msg_proto(const Packets::PacketClientMsgProtobuf& msg) {
    Steam::Internal::Enums::EMsg emsg = Steam::MsgUtil::get_msg(static_cast<uint32_t>(msg.MsgType()));
    switch (emsg) {
        case Steam::Internal::Enums::EMsg::Multi: {
            //parse_multi(msg);
            break;
        } case Steam::Internal::Enums::EMsg::ClientLogOnResponse: {
            ClientMessages::MsgProto<CMsgClientLogonResponse> response(msg);
            if (response.Body.eresult() == static_cast<uint32_t>(Steam::Internal::Enums::EResult::OK)) {
                emit(Steam::Events::ClientLogonEvent{});
            } else {
                logger->error("Logon to Steam servers failed");
            }
            break;
        }
        default: {
            logger->info("No handling for protobuf msg (EMsg {}) implemented.", static_cast<uint32_t>(emsg));
            break;
        }
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
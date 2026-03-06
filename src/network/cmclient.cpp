#include "network/cmclient.hpp"
#include <iostream>

// Connect via TCP, then start message loop
void Steam::Messaging::CMClient::start_session() {
    try {
        connection_->on_frame = [this](std::vector<uint8_t> frame) {
            consume_frame(frame);
        };

        setup_handlers();

        connection_->open_tcp();
        spdlog::info("Connected successfully.");
    } catch (const std::exception& e) {
        spdlog::error("Could not connect via TCP.");
    }
}

// Start consuming TCP messages
void Steam::Messaging::CMClient::consume_frame(std::vector<uint8_t> frame) {
    if (frame.size() < 4) return;

    uint32_t emsg = frame[0] | (frame[1] << 8) | (frame[2] << 16) | (frame[3] << 24);

    try {
        std::unique_ptr<Steam::Messaging::Packets::PacketMsg> packet;

        if (is_encryption_msg(emsg)) {
            auto proto = std::make_unique<Steam::Messaging::Packets::PacketClientMsgProtobuf>(static_cast<Steam::Internal::Enums::EMsg>(emsg), frame);
            emit(*proto);
        } else {
            auto raw   = std::make_unique<Steam::Messaging::Packets::PacketMsg>(static_cast<Steam::Internal::Enums::EMsg>(emsg), frame);
            emit(*raw);
        }

    } catch (const std::exception& e) {
        spdlog::error("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

void Steam::Messaging::CMClient::setup_handlers() {
    on<Steam::Messaging::Packets::PacketClientMsgProtobuf>([this](const Steam::Messaging::Packets::PacketClientMsgProtobuf& msg) {
        rcv_msg_proto(msg);
    });

    on<Steam::Messaging::Packets::PacketMsg>([this](const Steam::Messaging::Packets::PacketMsg& msg) {
        rcv_msg(msg);
    });
}

void Steam::Messaging::CMClient::rcv_msg_proto(const Steam::Messaging::Packets::PacketClientMsgProtobuf& msg) {
    spdlog::info("Received protobuf Msg (EMsg: {})", static_cast<uint32_t>(msg.MsgType()));
    spdlog::info("Payload size: {}", msg.payload.size());
    spdlog::info("Payload preview: {:02X} {:02X} {:02X} {:02X}", 
                 msg.payload.size() > 0 ? msg.payload[0] : 0,
                 msg.payload.size() > 1 ? msg.payload[1] : 0,
                 msg.payload.size() > 2 ? msg.payload[2] : 0,
                 msg.payload.size() > 3 ? msg.payload[3] : 0);
}

// Receives a PacketMsg with the normal MsgHdr
void Steam::Messaging::CMClient::rcv_msg(const Steam::Messaging::Packets::PacketMsg& msg) {
    uint32_t emsg = static_cast<uint32_t>(msg.MsgType());
    spdlog::info("Received raw Msg (EMsg: {})", emsg);
    spdlog::info("Payload size: {}", msg.payload.size());
    spdlog::info("Body starts at offset: {}", msg.bodyOffset);

    try {
        switch(msg.MsgType()) {
            case Steam::Internal::Enums::EMsg::ChannelEncryptRequest: {
                Steam::Messaging::ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResponse> response = 
                    crypto_.generate_encryption_response(msg);
                spdlog::info("Sending encryption response");
                spdlog::info("Response size (CMCLIENT): {}", response.Payload().size());
                send_msg(response);
                break;
            }
            case Steam::Internal::Enums::EMsg::ChannelEncryptResult: {
                Steam::Messaging::ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResult> res(msg);
                if (res.Body.result == Steam::Internal::Enums::EResult::OK) {
                    spdlog::info("Channel secured");
                    channel_secured_ = true;
                } else {
                    spdlog::info("Encryption handshake failed");
                }
                break;
            } 
            default: {
                spdlog::info("No handling for msg (EMsg {}) implemented.", emsg);
                break;
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("Failed to handle packet (EMsg {}): {}", emsg,e.what());
    }
}

/*
void Steam::Messaging::CMClient::send_msg_proto(const MsgProto& msg) {
    
}*/

template<typename TBody>
void Steam::Messaging::CMClient::send_msg(const Steam::Messaging::ClientMessages::Msg<TBody>& msg)
{
    auto buffer = msg.Serialize();

    spdlog::info("Sending RAW message (size: {}), (EMsg: {})", buffer.size(), static_cast<uint32_t>(msg.Body.GetEMsg()));
    connection_->async_send(buffer);
}

void Steam::Messaging::CMClient::shutdown() {
    if (connection_ && connection_->is_connected())
        connection_->close_tcp();
    
    spdlog::info("Closed TCP connection and CMClient successfully");
}
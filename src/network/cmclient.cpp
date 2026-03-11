#include "network/cmclient.hpp"
#include <iostream>

using namespace Steam::Messaging;

// Connect via TCP, then start message loop
void CMClient::start_session() {
    try {
        connection_->set_on_frame([this](const std::vector<uint8_t>& frame) {
            consume_frame(frame);
        });

        setup_handlers();

        connection_->network_connect();
        spdlog::info("Connected successfully.");
    } catch (const std::exception& e) {
        spdlog::error("Could not connect via TCP: {}",e.what());
    }
}

// Start consuming TCP messages
void CMClient::consume_frame(const std::vector<uint8_t>& frame) {
    if (frame.size() < 4) return;
    std::vector<uint8_t> data = frame;

    if (channel_secured_) {
        data = crypto_.process_incoming_encrypted_message(frame);
    }

    uint32_t emsg = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    try {
        if (is_encryption_msg(emsg)) {
            // We're a raw Msg
            auto raw = std::make_unique<Packets::PacketMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            emit(*raw);
        } else if (MsgUtil::is_protobuf_msg(emsg)) {
            // We're a protobuf Msg
            auto proto = std::make_unique<Packets::PacketClientMsgProtobuf>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            emit(*proto);
        } else {
            // We're a struct msg
            auto str   = std::make_unique<Packets::PacketClientMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            emit(*str);
        }

    } catch (const std::exception& e) {
        spdlog::error("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

void CMClient::setup_handlers() {
    on<Packets::PacketClientMsgProtobuf>([this](const Packets::PacketClientMsgProtobuf& msg) {
        rcv_msg_proto(msg);
    });

    on<Packets::PacketMsg>([this](const Packets::PacketMsg& msg) {
        rcv_msg(msg);
    });
}

void CMClient::rcv_msg_proto(const Packets::PacketClientMsgProtobuf& msg) {
    spdlog::info("Received protobuf Msg (EMsg: {})", static_cast<uint32_t>(msg.MsgType()));
    spdlog::info("Payload size: {}", msg.payload.size());
    spdlog::info("Payload preview: {:02X} {:02X} {:02X} {:02X}", 
                 msg.payload.size() > 0 ? msg.payload[0] : 0,
                 msg.payload.size() > 1 ? msg.payload[1] : 0,
                 msg.payload.size() > 2 ? msg.payload[2] : 0,
                 msg.payload.size() > 3 ? msg.payload[3] : 0);
}

// Receives a PacketMsg with the normal MsgHdr
void CMClient::rcv_msg(const Packets::PacketMsg& msg) {
    uint32_t emsg = static_cast<uint32_t>(msg.MsgType());
    spdlog::info("Received raw Msg (EMsg: {})", emsg);
    spdlog::info("Payload size: {}", msg.payload.size());
    spdlog::info("Body starts at offset: {}", msg.bodyOffset);


    try {
        switch(msg.MsgType()) {
            case Steam::Internal::Enums::EMsg::ChannelEncryptRequest: {
                ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResponse> response = 
                    crypto_.generate_encryption_response(msg);
                spdlog::info("Sending encryption response");
                spdlog::info("Response size (CMCLIENT): {}", response.Payload().size());
                send_msg(response);
                break;
            }
            case Steam::Internal::Enums::EMsg::ChannelEncryptResult: {
                ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResult> res(msg);
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

template<typename TBody>
void CMClient::send_msg(const ClientMessages::Msg<TBody>& msg)
{
    auto buffer = msg.Serialize();
    
    spdlog::info(
        "Sending RAW message (size: {}), (EMsg: {})", 
        buffer.size(), 
        static_cast<uint32_t>(msg.Body.GetEMsg())
    );

    if (channel_secured_) {
        buffer = crypto_.process_outgoing_encrypted_message(buffer);
    }

    connection_->async_send(buffer);
}

void CMClient::shutdown() {
    if (connection_ && connection_->is_connected())
        connection_->network_close();
    
    spdlog::info("Closed connection and CMClient successfully");
}
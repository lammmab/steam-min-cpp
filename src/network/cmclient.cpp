#include "network/cmclient.h"
#include <iostream>


// Connect via TCP, then start message loop
void CMClient::start_session() {
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
void CMClient::consume_frame(std::vector<uint8_t> frame) {
    if (frame.size() < 4) return;

    uint32_t emsg = frame[0] | (frame[1] << 8) | (frame[2] << 16) | (frame[3] << 24);

    try {
        std::unique_ptr<PacketMsg> packet;

        if (is_protobuf_msg(emsg)) {
            auto proto = std::make_unique<PacketClientMsgProtobuf>(static_cast<SteamInternal::EMsg>(emsg), frame);
            emit(*proto);
        } else {
            auto raw   = std::make_unique<PacketClientMsg>(static_cast<SteamInternal::EMsg>(emsg), frame);
            emit(*raw);
        }

    } catch (const std::exception& e) {
        spdlog::error("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

void CMClient::setup_handlers() {
    on<PacketClientMsgProtobuf>([this](const PacketClientMsgProtobuf& msg) {
        rcv_msg_proto(msg);
    });

    on<PacketClientMsg>([this](const PacketClientMsg& msg) {
        rcv_msg(msg);
    });
}

void CMClient::rcv_msg_proto(const PacketClientMsgProtobuf& msg) {
    spdlog::info("Received protobuf Msg (EMsg: {})", static_cast<uint32_t>(msg.MsgType()));
    spdlog::info("Payload size: {}", msg.payload.size());
    spdlog::info("Payload preview: {:02X} {:02X} {:02X} {:02X}", 
                 msg.payload.size() > 0 ? msg.payload[0] : 0,
                 msg.payload.size() > 1 ? msg.payload[1] : 0,
                 msg.payload.size() > 2 ? msg.payload[2] : 0,
                 msg.payload.size() > 3 ? msg.payload[3] : 0);
}

void CMClient::rcv_msg(const PacketClientMsg& msg) {
    spdlog::info("Received raw Msg (EMsg: {})", static_cast<uint32_t>(msg.MsgType()));
    spdlog::info("Payload size: {}", msg.payload.size());
    spdlog::info("Body starts at offset: {}", msg.bodyOffset);

    // TODO: handle encryption request
    // 1. Convert to proper ChannelEncryptRequest from SteamLanguageInternal.h
    // 2. generate_encryption_response(ChannelEncryptRequest)
    // 3. send response and store key
    try {
        EncryptionResponse response = generate_encryption_response(msg);
        spdlog::info("Sending encryption response");
        send_msg(response.msg);
    } catch (const std::exception& e) {
        spdlog::error("Failed to generate/send encryption response: {}", e.what());
    }
}

/*
void CMClient::send_msg_proto(const MsgProto& msg) {
    std::vector<uint8_t> buffer;

    buffer.resize(4);
    uint32_t raw_emsg = msg.emsg | 0x80000000;
    buffer.insert(buffer.end(),
                reinterpret_cast<uint8_t*>(&raw_emsg),
                reinterpret_cast<uint8_t*>(&raw_emsg) + 4);

    size_t header_len = msg.header.ByteSizeLong();
    std::vector<uint8_t> header_buf(header_len);
    msg.header.SerializeToArray(header_buf.data(), static_cast<int>(header_len));

    uint32_t header_len_le = static_cast<uint32_t>(header_len);
    buffer.insert(buffer.end(),
                reinterpret_cast<uint8_t*>(&header_len_le),
                reinterpret_cast<uint8_t*>(&header_len_le) + 4);

    buffer.insert(buffer.end(), header_buf.begin(), header_buf.end());
    buffer.insert(buffer.end(), msg.payload.begin(), msg.payload.end());

    uint32_t msg_len = static_cast<uint32_t>(buffer.size() - 4);
    std::memcpy(buffer.data(), &msg_len, 4);

    spdlog::info("Sending message (EMsg: {}, total size: {})", msg.emsg, buffer.size());
    connection_->async_send(buffer);
}*/

template<typename TBody>
void CMClient::send_msg(const Msg<TBody>& msg)
{
    auto buffer = msg.Serialize();

    spdlog::info("Sending RAW message (size: {}), (EMsg: {})", buffer.size(), static_cast<uint32_t>(msg.Body.GetEMsg()));
    connection_->async_send(buffer);
}

void CMClient::shutdown() {
    if (connection_ && connection_->is_connected())
        connection_->close_tcp();
    
    spdlog::info("Closed TCP connection and CMClient successfully");
}
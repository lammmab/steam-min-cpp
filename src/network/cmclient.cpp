#include "network/cmclient.h"
#include <iostream>
#include <protogen/enums_clientserver.pb.h>
#include <protogen/steammessages_clientserver_login.pb.h>

// Connect via TCP, then start message loop
void CMClient::start_session() {
    try {
        connection_->on_frame = [this](std::vector<uint8_t> frame) {
            consume_frame(frame);
        };
        connection_->open_tcp();
        spdlog::info("Connected successfully.");
    } catch (const std::exception& e) {
        spdlog::error("Could not connect via TCP.");
    }
}

// Start consuming TCP messages
void CMClient::consume_frame(std::vector<uint8_t> frame) {
    uint32_t emsg = frame[0] | (frame[1] << 8) | (frame[2] << 16) | (frame[3] << 24);
    try {
        if (is_protobuf_emsg(emsg)) {
            MsgProto msg;
            msg.parse(frame);
            emit(msg);
        } else {
            Msg msg;
            msg.parse(frame);
            emit(msg);
        }
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

/*
// TODO: k_EMsgChannelEncryptRequest
void CMClient::rcv_msg(const MsgProto& msg) {
    spdlog::info("Received MsgProto (EMsg: {})", msg.emsg);
    spdlog::info(msg.to_string());
}

void CMClient::rcv_msg(const Msg& msg) {
    spdlog::info("Received raw Msg (EMsg: {})", msg.emsg);
    spdlog::info(msg.to_string());

    if (msg.emsg == static_cast<uint32_t>(EMsg::k_EMsgChannelEncryptRequest)) {
        spdlog::info("Received CMsgChannelEncryptRequest!");
    }
}
*/

void CMClient::send_msg(const MsgProto& msg) {
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
}

void CMClient::shutdown() {
    if (connection_ && connection_->is_connected())
        connection_->close_tcp();
    
    spdlog::info("Closed TCP connection and CMClient successfully");
}
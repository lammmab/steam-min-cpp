#include "network/cmclient.h"
#include <iostream>
#include <protogen/enums_clientserver.pb.h>
#include <protogen/steammessages_clientserver_login.pb.h>

// Connect via TCP, then start message loop
void CMClient::start_session() {
    try {
        connection_->open();
        spdlog::info("Connected successfully.");
        msg_thread_ = std::thread([this]{ start_message_loop(); });
    } catch (const std::exception& e) {
        spdlog::error("Could not connect via TCP.");
    }
}

bool CMClient::is_tcp_connected() {
    return connection_->is_connected();
}

// Start consuming TCP messages
void CMClient::start_message_loop() {
    spdlog::info("Starting TCP message loop...");

    std::vector<uint8_t> buffer;
    size_t frame_count = 0;

    while (!stop_msg_thread_ && connection_->is_connected()) {
        while (connection_->has_message()) {
            auto frame = connection_->get_message();
            uint32_t emsg = frame[0] | (frame[1]<<8) | (frame[2]<<16) | (frame[3]<<24);
            frame_count++;
            spdlog::info("Processing frame #{} with {} bytes", frame_count, frame.size());

            try {
                if (is_protobuf_emsg(emsg)) {
                    MsgProto msg;
                    msg.parse(frame);
                    rcv_msg(msg);
                } else {
                    Msg msg;
                    msg.parse(frame);
                    rcv_msg(msg);
                }

                spdlog::info("Finished handling msg for frame #{}", frame_count);

            } catch (const std::exception& e) {
                spdlog::error("Failed to parse incoming message in frame #{}: {}", frame_count, e.what());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    spdlog::info("TCP message loop ended; processed {} frames.", frame_count);
}

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
    connection_->put_message(buffer);
}

void CMClient::shutdown() {
    stop_msg_thread_ = true;
    if (msg_thread_.joinable()) msg_thread_.join();
    connection_->close();
    spdlog::info("Closed TCP connection and CMClient successfully");
}
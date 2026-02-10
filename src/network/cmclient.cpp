#include "network/cmclient.h"
#include <iostream>
#include <protogen/enums_clientserver.pb.h>
#include <protogen/steammessages_clientserver_login.pb.h>

void CMClient::say_hello() {
    MsgProto hello;
    hello.emsg = static_cast<uint32_t>(EMsg::k_EMsgClientHello);

    hello.header.set_client_sessionid(0);
    hello.header.set_steamid(0);

    CMsgClientHello hello_payload;
    hello_payload.set_protocol_version(65575);
    std::string hello_bytes;
    hello_payload.SerializeToString(&hello_bytes);
    hello.payload.assign(hello_bytes.begin(), hello_bytes.end());
    send_msg(hello);
}

// Connect via TCP.
void CMClient::connect() {
    try {
        connection_->connect_or_throw();
        spdlog::info("Connected successfully.");
        msg_thread_ = std::thread([this]{ start_message_loop(); });
        say_hello();
    } catch (const std::exception& e) {
        spdlog::error("Could not connect via TCP.");
    }
}

size_t parse_message_length(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < 4) return 0;
    uint32_t len = 0;
    len |= static_cast<uint32_t>(buffer[0]);
    len |= static_cast<uint32_t>(buffer[1]) << 8;
    len |= static_cast<uint32_t>(buffer[2]) << 16;
    len |= static_cast<uint32_t>(buffer[3]) << 24;
    return static_cast<size_t>(len);
}

// Start consuming TCP messages
void CMClient::start_message_loop() {
    std::vector<uint8_t> buffer;
    while (connection_->is_connected()) {
        while (connection_->has_message()) {
            auto chunk = connection_->get_message();
            buffer.insert(buffer.end(), chunk.begin(), chunk.end());
        }

        while (true) {
            if (buffer.size() < 4) break;
            uint32_t frame_len = parse_message_length(buffer);
            if (buffer.size() < frame_len + 4) break;

            std::vector<uint8_t> frame(
                buffer.begin() + 4,
                buffer.begin() + 4 + frame_len
            );

            buffer.erase(buffer.begin(), buffer.begin() + 4 + frame_len);
            try {
                MsgProto msg;
                msg.parse(frame);
                rcv_msg(msg);
            } catch (const std::exception& e) {
                spdlog::error("Failed to parse message: {}", e.what());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CMClient::rcv_msg(const MsgProto& msg) {

}

void CMClient::send_msg(const MsgProto& msg) {
    std::vector<uint8_t> buffer;
    buffer.resize(8);
    uint32_t raw_emsg = msg.emsg | 0x80000000;
    std::memcpy(buffer.data() + 4, &raw_emsg, 4);

    size_t header_len = msg.header.ByteSizeLong();
    std::vector<uint8_t> header_buf(header_len);
    msg.header.SerializeToArray(header_buf.data(), static_cast<int>(header_len));

    buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&header_len),
                  reinterpret_cast<uint8_t*>(&header_len) + 4);

    buffer.insert(buffer.end(), header_buf.begin(), header_buf.end());

    buffer.insert(buffer.end(), msg.payload.begin(), msg.payload.end());

    uint32_t msg_len = static_cast<uint32_t>(buffer.size() - 4);
    std::memcpy(buffer.data(), &msg_len, 4);

    connection_->put_message(buffer);    
}

void CMClient::disconnect() {
    connection_->disconnect();
    if (msg_thread_.joinable()) msg_thread_.join();
} 

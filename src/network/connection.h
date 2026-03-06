#pragma once
#include <asio.hpp>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include "web/cmfetcher.h"
#include <spdlog/spdlog.h>
#include <optional>

namespace Steam::Networking {
    enum class ConnectionState{
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    };

    class TCPConnection {
    public:
        TCPConnection(asio::io_context& ctx);
        TCPConnection();

        ~TCPConnection();

        void open_tcp();
        void close_tcp();

        void async_send(std::vector<uint8_t> data);

        inline bool is_connected() const {
            return state_ == ConnectionState::CONNECTED;
        }
        std::function<void(std::vector<uint8_t>)> on_frame;
        
    private:
        void start_read_header();
        void start_read_body(uint32_t length);
        void do_write();

        void handle_disconnect(const std::string& reason);
        void handle_disconnect(const std::error_code& reason);

        Steam::Networking::Web::CMFetcher fetcher_;
        
        std::array<uint8_t, 8> header_buffer_;
        std::vector<uint8_t> body_buffer_;
        std::deque<std::vector<uint8_t>> write_queue_;
        
        ConnectionState state_ = ConnectionState::DISCONNECTED;
        
        const std::array<uint8_t, 4> MAGIC = {'V','T','0','1'};
            
        asio::io_context& ctx;
        asio::ip::tcp::socket socket_;
        std::function<void(const std::vector<uint8_t>&)> wait_for_callback;
    };
}
#pragma once
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
#include "connection.hpp"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace Steam::Networking::Web {
    class CMFetcher;
}

namespace Steam::Networking {
    class TCPConnection: public IConnection {
    public:
        TCPConnection(boost::asio::io_context& ctx);
        ~TCPConnection() override;

        inline ConnectionState state() const override {
            return state_;
        }

        void network_connect() override;
        void network_close() override;

        void async_send(const std::vector<uint8_t>& data) override;

        void set_on_frame(
            std::function<void(std::vector<uint8_t>)> handler
        ) override { on_frame_ = std::move(handler); }

    private:
        void start_read_header();
        void start_read_body(uint32_t length);
        void do_write();

        void handle_disconnect(const std::string& reason);
        void handle_disconnect(const std::error_code& reason);

        std::unique_ptr<Web::CMFetcher> fetcher_;

        
        std::array<uint8_t, 8> header_buffer_;
        std::vector<uint8_t> body_buffer_;
        std::deque<std::vector<uint8_t>> write_queue_;
        
        ConnectionState state_ = ConnectionState::DISCONNECTED;
        
        const std::array<uint8_t, 4> MAGIC = {'V','T','0','1'};
            
        boost::asio::io_context& ctx;
        boost::asio::ip::tcp::socket socket_;
        std::function<void(const std::vector<uint8_t>&)> wait_for_callback;
        std::function<void(std::vector<uint8_t>)> on_frame_;
        
    };
}
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

class TCPConnection {
public:
    TCPConnection(asio::io_context& ctx);
    ~TCPConnection();

    void connect_or_throw();
    void disconnect();

    void put_message(const std::vector<uint8_t>& data);
    bool has_message();
    std::vector<uint8_t> get_message();
    bool is_connected();
    private:
    void reader_loop();
    void writer_loop();
    void read_packets();
    CMFetcher fetcher_;
    
    asio::ip::tcp::socket socket_;
    std::thread reader_thread_;
    std::thread writer_thread_;
    
    std::vector<uint8_t> read_buffer_;
    std::queue<std::vector<uint8_t>> send_queue_;
    std::queue<std::vector<uint8_t>> recv_queue_;
    
    std::mutex send_mutex_;
    std::mutex recv_mutex_;
    std::condition_variable send_cv_;
    bool stop_threads_ = false;
    bool connected_ = false;
    asio::io_context& ctx;

    static constexpr uint8_t MAGIC[4] = {'V','T','0','1'};
};
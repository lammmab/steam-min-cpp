#include "connection.h"

TCPConnection::TCPConnection(asio::io_context& ctx)
    : socket_(ctx), ctx(ctx)
{
}

TCPConnection::~TCPConnection() {
    disconnect();
}

void TCPConnection::disconnect() {
    stop_threads_ = true;
    send_cv_.notify_all();

    if (socket_.is_open()) {
        std::error_code ec;
        socket_.close(ec);
    }

    if (reader_thread_.joinable()) reader_thread_.join();
    if (writer_thread_.joinable()) writer_thread_.join();

    connected_ = false;
}

void TCPConnection::connect_or_throw() {
    if (connected_) return;

    bool fetched_successfully = fetcher_.fetch_cm_servers();
    if (!fetched_successfully) {
        throw std::runtime_error("Could not fetch from CM servers");
    }

    std::vector<std::pair<std::string, int>> servers = fetcher_.get_servers();
    if (servers.empty()) {
        throw std::runtime_error("No CM servers found");
    }
    asio::ip::tcp::resolver resolver(ctx);
    std::exception_ptr last_error = nullptr;

    for (const auto& [host, port] : servers) {
        try {
            auto endpoints = resolver.resolve(host, std::to_string(port));

            asio::connect(socket_, endpoints);

            connected_ = true;

            reader_thread_ = std::thread([this]{ reader_loop(); });
            writer_thread_ = std::thread([this]{ writer_loop(); });
            
            return;
        } catch (const std::exception& e) {
            last_error = std::current_exception();
            std::cerr << "Failed to connect to " << host << ": " << e.what() << "\n";
        }
    }

    if (last_error) {
        connected_ = false;
        std::rethrow_exception(last_error);
    }
}

bool TCPConnection::is_connected() {
    return connected_;
}

void TCPConnection::put_message(const std::vector<uint8_t>& data) {
    {
        std::lock_guard<std::mutex> lock(send_mutex_);
        send_queue_.push(data);
    }
    send_cv_.notify_one();
}

bool TCPConnection::has_message() {
    std::lock_guard<std::mutex> lock(recv_mutex_);
    return !recv_queue_.empty();
}

std::vector<uint8_t> TCPConnection::get_message() {
    std::lock_guard<std::mutex> lock(recv_mutex_);
    if (recv_queue_.empty()) return {};
    auto msg = recv_queue_.front();
    recv_queue_.pop();
    return msg;
}

void TCPConnection::reader_loop() {
    try {
        while (connected_ && !stop_threads_) {
            std::error_code ec;
            size_t bytes_read = socket_.read_some(asio::buffer(read_buffer_), ec);

            if (ec == asio::error::eof) break;
            if (ec) throw asio::system_error(ec);

            std::vector<uint8_t> data(read_buffer_.begin(), read_buffer_.begin() + bytes_read);

            {
                std::lock_guard<std::mutex> lock(recv_mutex_);
                recv_queue_.push(std::move(data));
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("Reader loop terminated: {}", e.what());
    }

    connected_ = false;
    disconnect();
}

void TCPConnection::writer_loop() {
    try {
        while (connected_ && !stop_threads_) {
            std::unique_lock<std::mutex> lock(send_mutex_);
            send_cv_.wait(lock, [this]{ return !send_queue_.empty() || stop_threads_; });

            if (stop_threads_) break;

            auto data = std::move(send_queue_.front());
            send_queue_.pop();
            lock.unlock();

            std::error_code ec;
            asio::write(socket_, asio::buffer(data), ec);
            if (ec) throw asio::system_error(ec);
        }
    } catch (const std::exception& e) {
        spdlog::error("Writer loop terminated: {}", e.what());
        connected_ = false;
    }
}
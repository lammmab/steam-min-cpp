#include "network/tcp.hpp"
#include <spdlog/fmt/bin_to_hex.h>

uint32_t parse_message_length(const std::array<uint8_t, 8>& buffer) {
    if (buffer.size() < 4) {
        throw std::runtime_error("Buffer too small to parse length");
    }

    uint32_t len = 0;
    len |= static_cast<uint32_t>(buffer[0]);
    len |= static_cast<uint32_t>(buffer[1]) << 8;
    len |= static_cast<uint32_t>(buffer[2]) << 16;
    len |= static_cast<uint32_t>(buffer[3]) << 24;

    return len;
}

bool validate_magic(const std::array<uint8_t, 8>& buffer,const std::array<uint8_t, 4>& MAGIC) {
    for (size_t i = 0; i < MAGIC.size(); ++i) {
        if (buffer[4 + i] != MAGIC[i]) {
            return false;
        }
    }
    return true;
}

Steam::Networking::TCPConnection::TCPConnection(asio::io_context& ctx)
    : socket_(ctx), ctx(ctx) {}

Steam::Networking::TCPConnection::~TCPConnection() {
    if (is_connected()) network_close();
}

void Steam::Networking::TCPConnection::network_close()
{
    if (state_ == ConnectionState::DISCONNECTED)
        return;

    std::error_code ec;
    socket_.close(ec);

    state_ = ConnectionState::DISCONNECTED;
}

void Steam::Networking::TCPConnection::network_connect()
{
    if (state_ != ConnectionState::DISCONNECTED)
        return;

    spdlog::info("TCPConnection: starting open_tcp()");
    state_ = ConnectionState::CONNECTING;

    fetcher_.fetch_cm_servers();
    auto servers = fetcher_.get_servers();
    if (servers.empty()) {
        spdlog::error("TCPConnection: no CM servers found");
        throw std::runtime_error("No CM servers");
    }

    spdlog::info("TCPConnection: resolving server {}:{}", servers[0].first, servers[0].second);
    asio::ip::tcp::resolver resolver(ctx);
    auto endpoints = resolver.resolve(servers[0].first,
                                      std::to_string(servers[0].second));

    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, auto) {
            if (!ec) {
                state_ = ConnectionState::CONNECTED;
                spdlog::info("TCPConnection: connected, starting async read header");
                start_read_header();
            } else {
                state_ = ConnectionState::DISCONNECTED;
                spdlog::error("TCPConnection: failed to connect: {}", ec.message());
            }
        });
}

void Steam::Networking::TCPConnection::start_read_header()
{
    spdlog::info("TCPConnection: start_read_header() called");
    auto self = this;

    asio::async_read(socket_,
        asio::buffer(header_buffer_, 8),
        [this, self](std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
                spdlog::error("TCPConnection: header read failed: {}", ec.message());
                return handle_disconnect(ec);
            }

            spdlog::info("TCPConnection: header read {} bytes", bytes_transferred);
            uint32_t len = parse_message_length(header_buffer_);
            spdlog::info("TCPConnection: parsed frame length {}", len);

            if (!validate_magic(header_buffer_, MAGIC)) {
                spdlog::error("TCPConnection: magic validation failed");
                return handle_disconnect("Magic could not be verified");
            }

            spdlog::info("TCPConnection: magic validated, reading body of length {}", len);
            start_read_body(len);
        });
}

void Steam::Networking::TCPConnection::start_read_body(uint32_t len)
{
    spdlog::info("TCPConnection: start_read_body() called with len {}", len);
    body_buffer_.resize(len);

    asio::async_read(socket_,
        asio::buffer(body_buffer_),
        [this](std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
                spdlog::error("TCPConnection: body read failed: {}", ec.message());
                return handle_disconnect(ec);
            }

            spdlog::info("TCPConnection: body read {} bytes", bytes_transferred);
            if (on_frame_) {
                spdlog::info("TCPConnection: emitting on_frame callback");
                on_frame_(body_buffer_);
            } else {
                spdlog::warn("TCPConnection: on_frame callback is null");
            }

            spdlog::info("TCPConnection: looping to next header read");
            start_read_header();
        });
}

void Steam::Networking::TCPConnection::async_send(const std::vector<uint8_t>& data)
{
    uint32_t data_len = static_cast<uint32_t>(data.size());
    std::vector<uint8_t> buffer;

    buffer.reserve(sizeof(uint32_t) + MAGIC.size() + data.size());

    // https://github.com/SteamRE/SteamKit/blob/master/SteamKit2/SteamKit2/Networking/Steam3/TcpConnection.cs#L327
    // 1. Append the data len
    buffer.insert(buffer.end(),
                  reinterpret_cast<uint8_t*>(&data_len),
                  reinterpret_cast<uint8_t*>(&data_len) + sizeof(uint32_t));

    // 2. Append the MAGIC constant (VT01)
    buffer.insert(buffer.end(), MAGIC.begin(), MAGIC.end());

    // 3. Append the data
    buffer.insert(buffer.end(), data.begin(), data.end());

    spdlog::info("Sending this ENTIRE Buffer hex ({} bytes): {:spn}", buffer.size(), spdlog::to_hex(buffer));
    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(std::move(buffer));
    

    if (!write_in_progress)
        do_write();
}
void Steam::Networking::TCPConnection::do_write()
{
    asio::async_write(socket_,
        asio::buffer(write_queue_.front()),
        [this](std::error_code ec, std::size_t) {
            if (ec) return handle_disconnect(ec);

            write_queue_.pop_front();
            if (!write_queue_.empty())
                do_write();
        });
}

void Steam::Networking::TCPConnection::handle_disconnect(const std::string& reason) {
    spdlog::error("TCP disconnected: {}", reason);
    network_close();
}

void Steam::Networking::TCPConnection::handle_disconnect(const std::error_code& ec) {
    handle_disconnect(ec.message());
}
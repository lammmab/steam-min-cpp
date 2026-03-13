#include <steamclient/connections/tcp.hpp>
#include <boost/endian/conversion.hpp>
#include "utils/macros.h"

STEAMCLIENT_FILE_LOGGER();


using namespace Steam::Networking;
namespace asio = boost::asio;

uint32_t parse_message_length(const std::array<uint8_t, 8>& buffer) {
    if (buffer.size() < 4) {
        throw std::runtime_error("Buffer too small to parse length");
    }

    uint32_t len;
    std::memcpy(&len, buffer.data(), sizeof(uint32_t));

    return boost::endian::native_to_little(len);
}

bool validate_magic(const std::array<uint8_t, 8>& buffer,const std::array<uint8_t, 4>& MAGIC) {
    return std::equal(MAGIC.begin(), MAGIC.end(), buffer.begin() + 4);
}

TCPConnection::TCPConnection(asio::io_context& ctx)
    : socket_(ctx), ctx(ctx) {}

TCPConnection::~TCPConnection() {
    if (is_connected()) network_close();
}

void TCPConnection::network_close()
{
    if (state_ == ConnectionState::DISCONNECTED)
        return;

    boost::system::error_code ec;
    socket_.close(ec);

    state_ = ConnectionState::DISCONNECTED;
}

void TCPConnection::network_connect()
{
    if (state_ != ConnectionState::DISCONNECTED)
        return;

    state_ = ConnectionState::CONNECTING;

    fetcher_.fetch_cm_servers();
    auto servers = fetcher_.get_servers();
    if (servers.empty()) {
        throw std::runtime_error("No CM servers");
    }

    STEAMCLIENT_LOG_INFO("resolving server {}:{}", servers[0].first, servers[0].second);
    asio::ip::tcp::resolver resolver(ctx);
    auto endpoints = resolver.resolve(servers[0].first,
                                      std::to_string(servers[0].second));

    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, auto) {
            if (!ec) {
                state_ = ConnectionState::CONNECTED;
                start_read_header();
            } else {
                state_ = ConnectionState::DISCONNECTED;
            }
        });
}

void TCPConnection::start_read_header()
{
    asio::async_read(socket_,
        asio::buffer(header_buffer_, 8),
        [this](std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
                return handle_disconnect(ec);
            }

            uint32_t len = parse_message_length(header_buffer_);

            if (!validate_magic(header_buffer_, MAGIC)) {
                return handle_disconnect("Magic could not be verified");
            }

            start_read_body(len);
        });
}

void TCPConnection::start_read_body(uint32_t len)
{
    body_buffer_.resize(len);

    asio::async_read(socket_,
        asio::buffer(body_buffer_),
        [this](std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
                return handle_disconnect(ec);
            }

            if (on_frame_) {
                on_frame_(body_buffer_);
            }

            start_read_header();
        });
}

void TCPConnection::async_send(const std::vector<uint8_t>& data)
{
    uint32_t data_len = static_cast<uint32_t>(data.size());
    std::vector<uint8_t> buffer;

    buffer.reserve(sizeof(uint32_t) + MAGIC.size() + data.size());

    // https://github.com/SteamRE/SteamKit/blob/master/SteamKit2/SteamKit2/Networking/Steam3/TcpConnection.cs#L327
    // 1. Append the data length
    buffer.insert(buffer.end(),
                  reinterpret_cast<uint8_t*>(&data_len),
                  reinterpret_cast<uint8_t*>(&data_len) + sizeof(uint32_t));

    // 2. Append the MAGIC constant (VT01)
    buffer.insert(buffer.end(), MAGIC.begin(), MAGIC.end());

    // 3. Append the data
    buffer.insert(buffer.end(), data.begin(), data.end());

    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(std::move(buffer));
    

    if (!write_in_progress)
        do_write();
}

void TCPConnection::do_write()
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

void TCPConnection::handle_disconnect(const std::string& reason) {
    STEAMCLIENT_LOG_INFO("Disconnected: {}", reason);
    network_close();
}

void TCPConnection::handle_disconnect(const std::error_code& ec) {
    handle_disconnect(ec.message());
}
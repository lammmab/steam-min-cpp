#include <steamclient/connections/websockets.hpp>
#include "utils/macros.h"

STEAMCLIENT_FILE_LOGGER();

using namespace Steam::Networking;
namespace asio = boost::asio;

WebsocketConnection::WebsocketConnection(asio::io_context& ctx)
    : ctx_(ctx),ws_(ctx_) {}

WebsocketConnection::~WebsocketConnection() {
    if (is_connected()) network_close();
}

void WebsocketConnection::reader_loop() {
    boost::system::error_code ec;
    auto buffer = asio::dynamic_vector_buffer(buffer_);
    
    ws_.async_read(
        buffer, 
        [this](boost::system::error_code ec, std::size_t bytes_transferred){
            if (ec) {
                STEAMCLIENT_LOG_ERROR("read failed: {}", ec.message());
                return handle_disconnect(ec);
            }
            
            STEAMCLIENT_LOG_INFO("read {} bytes", bytes_transferred);
            if (on_frame_) {
                STEAMCLIENT_LOG_INFO("emitting on_frame callback");
                on_frame_(buffer_);
            } else {
                STEAMCLIENT_LOG_WARN("on_frame callback is null");
            }

            reader_loop();
    });
}

void WebsocketConnection::network_connect() {
    if (state_ != ConnectionState::DISCONNECTED)
        return;

    STEAMCLIENT_LOG_INFO("starting network_connect()");
    state_ = ConnectionState::CONNECTING;

    fetcher_.fetch_cm_servers();
    auto servers = fetcher_.get_servers();
    if (servers.empty()) {
        throw std::runtime_error("No CM servers");
    }

    STEAMCLIENT_LOG_INFO("resolving server {}:{}", servers[0].first, servers[0].second);
    asio::ip::tcp::resolver resolver(ctx_);

    auto const endpoints = resolver.resolve(servers[0].first,
                                      std::to_string(servers[0].second));
    std::string host = servers[0].first;

    asio::async_connect(ws_.next_layer(), endpoints,
    [this, host](std::error_code ec, auto) {
        if (!ec) {
            ws_.async_handshake(host, "/", [this](boost::system::error_code ec) {
                state_ = ConnectionState::CONNECTED;
                if (!ec) reader_loop();
            });
        } else {
            state_ = ConnectionState::DISCONNECTED;
            STEAMCLIENT_LOG_ERROR("Failed to connect: {}", ec.message());
        }
    });
}

void WebsocketConnection::network_close() {
    if (state_ == ConnectionState::DISCONNECTED)
        return;

    ws_.close(boost::beast::websocket::close_code::normal);
    state_ = ConnectionState::DISCONNECTED;
}

void WebsocketConnection::async_send(const std::vector<uint8_t>& data) {
    STEAMCLIENT_LOG_ERROR("async_send Unimplemented");
}

void WebsocketConnection::handle_disconnect(const std::string& reason) {
    STEAMCLIENT_LOG_INFO("Connection disconnected: {}", reason);
    network_close();
}

void WebsocketConnection::handle_disconnect(const std::error_code& ec) {
    handle_disconnect(ec.message());
}
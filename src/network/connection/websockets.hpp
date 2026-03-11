#include "network/connection/connection.hpp"
#include "web/cmfetcher.hpp"
#include <boost/beast.hpp>
#include <cstring>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <boost/system/error_code.hpp>

namespace Steam::Networking {
    class WebsocketConnection : public IConnection {
    public:
        WebsocketConnection(boost::asio::io_context& ctx);
        ~WebsocketConnection() override;

        inline ConnectionState state() const override {
            return state_;
        }

        void network_connect() override;
        void network_close() override;

        void async_send(const std::vector<uint8_t>& data) override;

        void set_on_frame(
            std::function<void(std::vector<uint8_t>)> handler
        ) { on_frame_ = std::move(handler); }

        void reader_loop();

    private:
        void handle_disconnect(const std::string& reason);
        void handle_disconnect(const std::error_code& reason);

        boost::asio::io_context& ctx_;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;
        ConnectionState state_ = ConnectionState::DISCONNECTED;
        std::function<void(std::vector<uint8_t>)> on_frame_;
        Steam::Networking::Web::CMFetcher fetcher_;
        std::vector<uint8_t> buffer_;

    };
}
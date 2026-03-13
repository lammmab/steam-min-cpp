#pragma once
#include <vector>
#include <cstdint>
#include <functional>

namespace Steam::Networking {
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    };

    class IConnection {
        public:
            virtual ~IConnection() = default;
            virtual ConnectionState state() const = 0;
            inline bool is_connected() const {
                return state() == ConnectionState::CONNECTED;
            }

            virtual void network_connect() = 0;
            virtual void network_close() = 0;

            virtual void async_send(const std::vector<uint8_t>& data) = 0;
            virtual void set_on_frame(
                std::function<void(std::vector<uint8_t>)> handler
            ) = 0;
    };
}
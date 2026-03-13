/// @file connection.hpp
/// @brief Abstract transport interface used by the Steam networking layer.

#pragma once
#include <vector>
#include <cstdint>
#include <functional>

namespace Steam::Networking {

    /// @brief Connection lifecycle state.
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    };

    /// @brief Transport interface used by the CM networking client.
    ///
    /// Implementations provide the underlying network transport
    /// (e.g. TCP, WebSocket). The Steam client interacts with this
    /// interface to send and receive raw message frames.
    class IConnection {
    public:

        virtual ~IConnection() = default;

        /// @brief Retrieve the current connection state.
        virtual ConnectionState state() const = 0;

        /// @brief Check whether the transport is connected.
        inline bool is_connected() const {
            return state() == ConnectionState::CONNECTED;
        }

        /// @brief Initiate the network connection.
        ///
        /// Implementations should begin establishing the underlying
        /// transport connection.
        virtual void network_connect() = 0;

        /// @brief Close the active connection.
        ///
        /// Implementations should terminate the transport and release
        /// any associated resources.
        virtual void network_close() = 0;

        /// @brief Send a raw message frame.
        ///
        /// @param data Serialized packet payload to transmit.
        virtual void async_send(const std::vector<uint8_t>& data) = 0;

        /// @brief Register a frame receive handler.
        ///
        /// The handler is invoked whenever a full message frame is
        /// received from the transport.
        ///
        /// @param handler Callback receiving the raw frame bytes.
        virtual void set_on_frame(
            std::function<void(std::vector<uint8_t>)> handler
        ) = 0;
    };
}
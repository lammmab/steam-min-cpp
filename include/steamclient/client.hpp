/// @file client.hpp
/// @brief Steamworks minimal C++ wrapper

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>

#include <steamclient/connections/connection.hpp>

namespace Steam::Messaging {
    class CMClient;
}

/// @defgroup steam_client Steam Client
/// High-level interface used to communicate with Steam CM servers.
///
/// The internal client manages the network session, event dispatch system,
/// and command routing.
///
/// Usage flow:
/// 1. Construct the client with a connection backend.
/// 2. Register event callbacks.
/// 3. Connect to a CM server.
/// 4. Send commands and process events.
/// @{
namespace Steam {
    class SteamClient {
    public:

        /// @name Construction and Destruction
        /// Client lifetime management.
        /// @{

        /// Construct a Steam client using a specific connection implementation.
        /// @param connection Connection backend used for communicating with Steam CM servers.
        SteamClient(std::unique_ptr<Steam::Networking::IConnection> connection);

        /// Destructor
        /// Ensures the client disconnects from CM servers before destruction.
        ~SteamClient();

        /// @}

        /// @name Connection Management
        /// Establishing and terminating CM sessions.
        /// @{

        /// Establish a session with a Steam CM server.
        void connect();

        /// Disconnect from the current Steam CM server session.
        void disconnect();

        /// Check whether the client is currently connected.
        /// @return True if a CM session is currently active.
        bool is_connected() const;

        /// @}
        
        /// @name Event System
        /// Register callbacks for events emitted by the CM client.
        /// @{

        /// Registers a callback for a specific CMClient event.
        ///
        /// Subscribes a handler to the underlying CMClient dispatch system.
        /// The callback will be invoked whenever an event of type `Event`
        /// is emitted by the internal network client.
        ///
        /// @tparam Type Event type to listen for.
        /// @tparam Fn   Callable type used as the event handler.
        ///
        /// @param callback Callable invoked when the specified event occurs.
        ///                 The callable should accept the emitted event object.
        template<typename Type, typename Fn>
        void on(Fn&& callback);

        /// @}

        /// @name Command Execution
        /// Sending requests to the Steam CM server.
        /// @{

        /// Send a command to the Steam CM server.
        ///
        /// Routes a request object through the underlying CMClient command
        /// dispatcher. Most commands produce a response message, which can
        /// be handled by registering an event listener with `on`.
        ///
        /// @tparam Request Type representing the CM request message.
        /// @param req Request structure to send to the server.
        template<typename Request>
        void execute(const Request& req);

        /// @}

    private:
        std::unique_ptr<Steam::Messaging::CMClient> network_;
    };
}
/// @}

#include "client.inl"
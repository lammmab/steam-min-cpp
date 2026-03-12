#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "network/cmclient.hpp"

namespace Steam {
    class SteamClient {
    public:
        SteamClient(std::unique_ptr<Steam::Networking::IConnection> connection)
            : network_(std::move(connection)) {}

        ~SteamClient() {
            disconnect();
        }

        // TCP Connection / CMClient Delegation

        // Connect to a Steam CM server via. the underlying connection
        inline void connect() {
            network_.start_session();
        };

        // Disconnects from the connected Steam CM server via. the underlying connection
        inline void disconnect() {
            network_.shutdown();
        };

        // Returns a bool corresponding to whether or not you are connected to a Steam CM server.
        inline bool is_connected() const {
            return network_.is_connected();
        };
        
        // Connects to an emitted CMClient event.
        template<typename Type, typename Fn>
        inline void on(Fn&& callback)
        {
            network_.on<Type>(std::forward<Fn>(callback));
        }

        // Executes a command to the Steam CM server.
        template<typename Request>
        inline void execute(const Request& req) {
            network_.execute(req);
        }

    private:
        Steam::Messaging::CMClient network_;
    };
}
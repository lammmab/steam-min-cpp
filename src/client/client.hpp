#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "network/cmclient.hpp"
#include "auth/auth.hpp"
namespace Steam {
    class SteamClient {
    public:
        SteamClient(std::unique_ptr<Steam::Networking::IConnection> connection)
            : network_(std::move(connection)) {}

        ~SteamClient() {
            if (auth_.logged_in()) logout();
            if (network_.is_connected()) disconnect();
        }

        // TCP Connection / CMClient Delegation
        inline void connect() {
            network_.start_session();
        };
        inline void disconnect() {
            network_.shutdown();
        };
        inline bool is_connected() const {
            return network_.is_connected();
        };
        
        template<typename Type, typename Fn>
        inline void client_on(Fn&& callback)
        {
            network_.on<Type>(std::forward<Fn>(callback));
        }

        // Authorization Delegation
        inline bool anonymous_login() {
            return auth_.anonymous_login(network_);
        };
        inline void logout() {
            auth_.logout(network_);
        };
        inline bool logged_on() const {
            return auth_.logged_in();
        };

    private:
        Steam::Messaging::CMClient network_;
        Steam::Authentication::Auth auth_;
    };
}
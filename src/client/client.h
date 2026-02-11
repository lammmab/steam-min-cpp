#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "network/cmclient.h"
#include "auth/auth.h"


class SteamClient {
public:
    SteamClient(asio::io_context& ctx)
        : network_(ctx) {}

    ~SteamClient() {
        if (auth_.logged_in()) logout();
        if (network_.is_tcp_connected()) disconnect();
    }

    // TCP Connection / CMClient Delegation
    inline void connect() {
        network_.start_session();
    };
    inline void disconnect() {
        network_.shutdown();
    };
    inline bool is_connected() const {
        return network_.is_tcp_connected();
    };

    // Authorization Delegation
    inline bool anonymous_login() {
        return auth_.anonymous_login();
    };
    inline void logout() {
        auth_.logout();
    };
    inline bool logged_on() const {
        return auth_.logged_in();
    };
private:
    CMClient network_;
    Auth auth_;
};
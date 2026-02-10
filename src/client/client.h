#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "network/cmclient.h"
#include "auth/auth.h"

struct Placeholder {};

class SteamClient {
public:
    SteamClient(asio::io_context& ctx);
    ~SteamClient();

    void connect();
    void disconnect();
    bool is_connected() const;

    bool anonymous_login();
    void logout();
    bool logged_on() const;
private:
    CMClient network_;
    Auth auth_;
    Placeholder product_info_;
    Placeholder vdf_parser_;

    bool logged_on_;
    uint64_t current_jobid_;
};
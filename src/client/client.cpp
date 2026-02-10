// Written by lammmab. Specifically for use with accela-cpp.
// Basically a minimal port of the Python Steam client.
#include "client/client.h"

SteamClient::SteamClient(asio::io_context& ctx)
    : logged_on_(false), current_jobid_(0), network_(ctx) {}

SteamClient::~SteamClient() {
    if (logged_on_) logout();
    disconnect();
}

void SteamClient::connect() {
    network_.connect();
}

void SteamClient::disconnect() {
    network_.disconnect();
}

void SteamClient::logout() {
    
}
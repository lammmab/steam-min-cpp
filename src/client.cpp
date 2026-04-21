#include <steamclient/client.hpp>

namespace Steam {
SteamClient::SteamClient(std::unique_ptr<Networking::IConnection> connection) {
  network_ = std::make_unique<Messaging::CMClient>(std::move(connection));
}

SteamClient::~SteamClient() { disconnect(); }

void SteamClient::connect() { network_->start_session(); }

void SteamClient::disconnect() { network_->shutdown(); }

bool SteamClient::is_connected() const { return network_->is_connected(); }

}  // namespace Steam
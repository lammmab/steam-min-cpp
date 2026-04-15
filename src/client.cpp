#include <steamclient/client.hpp>
#include "network/cmclient.hpp"

namespace Steam {
    SteamClient::SteamClient(std::unique_ptr<Networking::IConnection> connection)
    {
        network_ = std::make_unique<Messaging::CMClient>(std::move(connection));
    }

    SteamClient::~SteamClient()
    {
        disconnect();
    }

    void SteamClient::connect()
    {
        network_->start_session();
    }

    void SteamClient::disconnect()
    {
        network_->shutdown();
    }

    bool SteamClient::is_connected() const
    {
        return network_->is_connected();
    }

    void SteamClient::on_impl(
        std::type_index type,
        std::function<void(const void*)> handler)
    {
        network_->on_erased(type, std::move(handler));
    }

    void SteamClient::execute_impl(std::type_index type, const void* req) {
        network_->emit_erased(type, req);
    }
}
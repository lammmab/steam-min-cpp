#include "network/cmclient.hpp"

namespace Steam {

    template<typename Type, typename Fn>
    void SteamClient::on(Fn&& callback) {
        network_->template on<Type>(std::forward<Fn>(callback));
    }

    template<typename Request>
    void SteamClient::execute(const Request& req) {
        network_->execute(req);        
    }
}
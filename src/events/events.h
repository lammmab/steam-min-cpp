#pragma once

namespace Steam::Events {
    struct ChannelSecuredEvent {};
    struct ClientLogonEvent {};
}

namespace Steam::Commands {
    struct AnonymousLogin {};
    struct GetProductInfo {
        int id;
    };
}
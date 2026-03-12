#pragma once

#include <string>

namespace Steam::Events {
    struct EventResult {
        bool success;
        std::string what; // If not success; this will be filled.
    };
    struct ChannelSecuredEvent {
        EventResult result;
    };
    struct ClientLogonEvent {
        EventResult result;
    };
}

namespace Steam::Commands {
    struct AnonymousLogin {};
    struct GetProductInfo {
        int id;
    };
}
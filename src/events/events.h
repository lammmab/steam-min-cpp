#pragma once

#include <string>

namespace Steam::Events {
    struct EventResult {
        bool success;
        std::string what; // If not success; this will be filled.
        static EventResult ok() {
            return {true, {}};
        }

        static EventResult fail(std::string msg) {
            return {false, std::move(msg)};
        }
    };
    struct Event {
        EventResult result;

        Event() = default;
        Event(EventResult r) : result(std::move(r)) {}

        bool ok() const { return result.success; }
        const std::string& what() const { return result.what; }
    };

    struct ChannelSecuredEvent : Event {};
    struct ClientLogonEvent : Event {};
}

namespace Steam::Commands {
    struct AnonymousLogin {};
    struct ClientLogoff {};
    struct GetProductInfo {
        int id;
    };
}
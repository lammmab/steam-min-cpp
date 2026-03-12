#pragma once

#include <array>
#include <cstddef>
#include <typeindex>
#include "events/events.h"
#include "base/clientmsg.hpp"

namespace Steam::Messaging {
    class CMClient;
}

namespace Steam::Dispatch {
    constexpr size_t MAX_REQUESTS = 512;
    using RequestExecFn = 
        void(*)(Steam::Messaging::CMClient&, const void*);

    struct RequestRouter {
        std::array<RequestExecFn, MAX_REQUESTS> table{};
    };

    inline RequestRouter g_request_router;

    inline size_t next_request_id() {
        static size_t id = 0;
        return id++;
    }

    template<typename T>
    size_t request_id() { 
        static size_t id = next_request_id();
        return id;
    }
}
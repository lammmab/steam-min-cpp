#pragma once

#include "commands/router.hpp"

namespace Steam::Dispatch {
    template<typename Request, auto Fn>
    struct RequestRegister
    {
        RequestRegister()
        {
            size_t id = request_id<Request>();

            g_request_router.table[id] =
                [](Steam::Messaging::CMClient& client, const void* req)
                {
                    Fn(client, *static_cast<const Request*>(req));
                };
        }
    };
}
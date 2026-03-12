#pragma once

#include "dispatch/table.hpp"
#include "base/generated/SteamLanguage.hpp"

namespace Steam::Dispatch {

    template<Steam::Internal::Enums::EMsg Msg,auto Fn>
    struct ProtoRegister
    {
        ProtoRegister()
        {
            g_dispatch.proto[static_cast<uint32_t>(Msg)] = Fn;
        }
    };

    template<Steam::Internal::Enums::EMsg Msg,auto Fn>
    struct MsgRegister
    {
        MsgRegister()
        {
            g_dispatch.msg[static_cast<uint32_t>(Msg)] = Fn;
        }
    };

}
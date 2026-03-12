#pragma once

#include "dispatch/table.hpp"
#include "base/generated/SteamUtils.hpp"
#include "base/packetbase.hpp"

namespace Steam::Dispatch {
    inline void dispatch_proto(
        Steam::Messaging::CMClient& client,
        const Steam::Messaging::Packets::PacketClientMsgProtobuf& packet) {
        
        Steam::Internal::Enums::EMsg emsg = Steam::MsgUtil::get_msg(static_cast<uint32_t>(packet.MsgType()));
        uint32_t id = static_cast<uint32_t>(emsg);
        if (id < MAX_EMSG)
        {
            auto fn = g_dispatch.proto[id];
            if (fn)
                fn(client, packet);
        }
    }

    inline void dispatch_msg(
        Steam::Messaging::CMClient& client,
        const Steam::Messaging::Packets::PacketMsg& packet)
    {
        uint32_t id = static_cast<uint32_t>(packet.MsgType());
        if (id < MAX_EMSG)
        {
            auto fn = g_dispatch.msg[id];
            if (fn) fn(client, packet);
        }
    }
}
#pragma once

#include "dispatch/table.hpp"
#include "utils/steam_utils.hpp"
#include "base/packetbase.hpp"

namespace Steam::Messaging {
    class CMClient;
}

namespace Steam::Dispatch {
    void dispatch_proto(
        Steam::Messaging::CMClient& client,
        const Steam::Messaging::Packets::PacketClientMsgProtobuf& packet
    );

    void dispatch_msg(
        Steam::Messaging::CMClient& client,
        const Steam::Messaging::Packets::PacketMsg& packet
    );
}
/// @internal
#pragma once

#include <steamclient/routing/events/table.hpp>
#include <steamclient/types/packetbase.hpp>
#include <steamclient/utilities/stream.hpp>

namespace Steam::Messaging {
class CMClient;
}

namespace Steam::Dispatch {
void dispatch_proto(
    Steam::Messaging::CMClient& client,
    const Steam::Messaging::Packets::PacketClientMsgProtobuf& packet);

void dispatch_msg(Steam::Messaging::CMClient& client,
                  const Steam::Messaging::Packets::PacketMsg& packet);
}  // namespace Steam::Dispatch
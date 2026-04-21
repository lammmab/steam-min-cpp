#include <steamclient/events.hpp>
#include <steamclient/network/cmclient.hpp>
#include <steamclient/routing/events/dispatcher.hpp>

namespace Steam::Dispatch {
void dispatch_proto(
    Steam::Messaging::CMClient& client,
    const Steam::Messaging::Packets::PacketClientMsgProtobuf& packet) {
  Steam::Internal::Enums::EMsg emsg =
      Steam::Utils::MsgUtil::get_msg(static_cast<uint32_t>(packet.MsgType()));

  uint32_t id = static_cast<uint32_t>(emsg);

  if (id <= MAX_EMSG) {
    auto fn = g_dispatch.proto[id];
    if (fn) {
      fn(client, packet);
    } else {
      client.emit(Steam::Events::MsgNotImplementedEvent{id, &packet});
    }
  }
}

void dispatch_msg(Steam::Messaging::CMClient& client,
                  const Steam::Messaging::Packets::PacketMsg& packet) {
  uint32_t id = static_cast<uint32_t>(packet.MsgType());

  if (id <= MAX_EMSG) {
    auto fn = g_dispatch.msg[id];

    if (fn) {
      fn(client, packet);
    } else {
      client.emit(Steam::Events::MsgNotImplementedEvent{id, &packet});
    }
  }
}
}  // namespace Steam::Dispatch
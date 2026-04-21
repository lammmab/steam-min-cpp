#include <steamclient/network/cmclient.hpp>
#include <steamclient/routing/events/dispatcher.hpp>
#include <steamclient/routing/events/register.hpp>

#include "steammessages_clientserver_login.pb.h"

using namespace Steam;
using namespace Steam::Messaging;

static void handle_logon_response(
    CMClient& client, const Packets::PacketClientMsgProtobuf& packet) {
  ClientMessages::MsgProto<CMsgClientLogonResponse> msg(packet);

  auto result = static_cast<Internal::Enums::EResult>(msg.Body.eresult());

  bool success = result == Internal::Enums::EResult::OK;

  Steam::Events::ClientLogonEvent evt{
      success ? Steam::Events::EventResult::ok()
              : Steam::Events::EventResult::fail(
                    "Client login failed; incorrect information?"),
      msg.Body.client_supplied_steamid(), msg.Header.proto.client_sessionid()};

  client.emit(evt);
}

static Steam::Dispatch::ProtoRegister<
    Internal::Enums::EMsg::ClientLogOnResponse, handle_logon_response>
    reg_logon;
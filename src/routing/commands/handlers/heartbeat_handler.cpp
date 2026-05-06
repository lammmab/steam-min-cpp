#include <steamclient/network/cmclient.hpp>
#include <steamclient/routing/commands/register.hpp>

#include "steamclient/types/generated/SteamLanguage.hpp"
#include "steammessages_clientserver_login.pb.h"

using namespace Steam::Messaging;
using namespace Steam::Commands;
namespace Enums = Steam::Internal::Enums;

static void exec_heartbeat(CMClient& client, const Heartbeat& req) {
  if (!client.is_channel_secured()) return;
  ClientMessages::MsgProto<CMsgClientHeartBeat> msg(Enums::EMsg::Heartbeat);

  client.send_msg(msg);
}

static Steam::Dispatch::RequestRegister<Heartbeat, exec_heartbeat>
    reg_heartbeat;
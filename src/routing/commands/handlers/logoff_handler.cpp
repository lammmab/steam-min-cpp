#include <steamclient/network/cmclient.hpp>
#include <steamclient/routing/commands/register.hpp>
#include <steamclient/utilities/msg_utils.hpp>

#include "steammessages_clientserver_login.pb.h"


using namespace Steam::Messaging;
using namespace Steam::Commands;
namespace Enums = Steam::Internal::Enums;

static void exec_logoff(CMClient& client, const ClientLogoff& req) {
  if (!client.is_channel_secured()) return;

  ClientMessages::MsgProto<CMsgClientLogon> msg(Enums::EMsg::ClientLogOff);
  client.send_msg(msg);
}

static Steam::Dispatch::RequestRegister<ClientLogoff, exec_logoff> reg_logoff;
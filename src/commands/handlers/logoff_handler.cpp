#include "commands/register.hpp"
#include "protogen/steammessages_clientserver_login.pb.h"
#include "utils/steam_utils.hpp"

#include "network/cmclient.hpp"

using namespace Steam::Messaging;
using namespace Steam::Commands;
namespace Enums = Steam::Internal::Enums;

static void exec_logoff(CMClient& client, const ClientLogoff& req) {
    if (!client.is_channel_secured()) return;
    
    ClientMessages::MsgProto<CMsgClientLogon> msg(Enums::EMsg::ClientLogOff);
    client.send_msg(msg);
}

static Steam::Dispatch::RequestRegister<
    ClientLogoff,
    exec_logoff
> reg_logoff;
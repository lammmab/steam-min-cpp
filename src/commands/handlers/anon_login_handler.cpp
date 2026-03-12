#include "commands/register.hpp"
#include "protogen/steammessages_clientserver_login.pb.h"
#include "base/generated/SteamUtils.hpp"
#include "base/generated/SteamUtils.hpp"

#include "network/cmclient.hpp"

using namespace Steam::Messaging;
using namespace Steam::Commands;
namespace Enums = Steam::Internal::Enums;

static void exec_anon_logon(CMClient& client, const AnonymousLogin& req) {
    ClientMessages::MsgProto<CMsgClientLogon> msg(Enums::EMsg::ClientLogon);

    SteamID steamID(
        0,
        0,
        Steam::Internal::Enums::EUniverse::Public,
        Steam::Internal::Enums::EAccountType::AnonUser
    );

    msg.Header.proto.set_steamid(steamID.ConvertToUInt64());
    msg.Body.set_client_package_version(1561159470);
    msg.Body.set_protocol_version(65580);

    client.send_msg(msg);
}

static Steam::Dispatch::RequestRegister<
    AnonymousLogin,
    exec_anon_logon
> reg_logon;
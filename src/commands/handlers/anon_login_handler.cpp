#include "commands/register.hpp"
#include "protogen/steammessages_clientserver_login.pb.h"

#include "network/cmclient.hpp"

using namespace Steam::Messaging;
using namespace Steam::Commands;
namespace Enums = Steam::Internal::Enums;
namespace Events = Steam::Events;

static void exec_anon_logon(CMClient& client, const AnonymousLogin& req) {
    if (!client.is_channel_secured()) {
        client.emit(Events::ClientLogonEvent{
            Events::EventResult::fail("Channel not secured")
        });
    } 

    ClientMessages::MsgProto<CMsgClientLogon> msg(Enums::EMsg::ClientLogon);

    Steam::Internal::SteamID steamID(
        0,
        0,
        Enums::EUniverse::Public,
        Enums::EAccountType::AnonUser
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
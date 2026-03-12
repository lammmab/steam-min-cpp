#include "auth/auth.hpp"
#include "protogen/steammessages_clientserver_login.pb.h"
#include "base/generated/SteamUtils.hpp"

using namespace Steam::Authentication;

namespace ClientMessages = Steam::Messaging::ClientMessages;

void Auth::logout(Steam::Messaging::CMClient& client) {
    if (!logged_in_) return;
    if (!client.is_channel_secured()) return;

    username_ = "";
    access_token_ = "";

    ClientMessages::MsgProto<CMsgClientLogOff> msg(Steam::Internal::Enums::EMsg::ClientLogOff);
    client.send_msg(msg);
}

bool Auth::anonymous_login(Steam::Messaging::CMClient& client) {
    if (!client.is_channel_secured()) return false;
    username_ = "";
    access_token_ = "";

    ClientMessages::MsgProto<CMsgClientLogon> msg(Steam::Internal::Enums::EMsg::ClientLogon);

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
    return true; // Return true for now even though we have no idea if it actually connected for testing
}
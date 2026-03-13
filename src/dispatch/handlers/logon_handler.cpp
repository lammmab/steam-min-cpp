#include "dispatch/register.hpp"
#include "network/cmclient.hpp"

#include "protogen/steammessages_clientserver_login.pb.h"

using namespace Steam;
using namespace Steam::Messaging;

static void handle_logon_response(
    CMClient& client,
    const Packets::PacketClientMsgProtobuf& packet)
{
    ClientMessages::MsgProto<CMsgClientLogonResponse> msg(packet);

    auto result =
        static_cast<Internal::Enums::EResult>(msg.Body.eresult());
    
    bool success = result == Internal::Enums::EResult::OK;

    client.emit(Steam::Events::ClientLogonEvent{
        success
            ? Steam::Events::EventResult::ok()
            : Steam::Events::EventResult::fail("Client login failed; incorrect information?"),
        msg.Body.client_supplied_steamid(),
        msg.Header.proto.client_sessionid()
    });
}

static Steam::Dispatch::ProtoRegister<
    Internal::Enums::EMsg::ClientLogOnResponse,
    handle_logon_response
> reg_logon;
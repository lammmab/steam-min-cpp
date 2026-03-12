#include "dispatch/register.hpp"
#include "network/cmclient.hpp"

#include "protogen/steammessages_clientserver_login.pb.h"
#include "base/generated/SteamUtils.hpp"

#include "events/events.h"

using namespace Steam;
using namespace Steam::Messaging;

static void handle_logon_response(
    CMClient& client,
    const Packets::PacketClientMsgProtobuf& packet)
{
    ClientMessages::MsgProto<CMsgClientLogonResponse> msg(packet);

    auto result =
        static_cast<Internal::Enums::EResult>(msg.Body.eresult());
    
    client.emit(Steam::Events::ClientLogonEvent{
        result: Steam::Events::EventResult{
            success: result == Internal::Enums::EResult::OK
        }
    });
}

static Steam::Dispatch::ProtoRegister<
    Steam::Internal::Enums::EMsg::ClientLogOnResponse,
    handle_logon_response
> reg_logon;
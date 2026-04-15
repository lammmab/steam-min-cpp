#include "dispatch/register.hpp"
#include "network/cmclient.hpp"

using namespace Steam;
using namespace Steam::Messaging;
namespace Events = Steam::Events;
namespace Enums = Steam::Internal::Enums;

static void handle_encrypt_request(
    CMClient& client,
    const Packets::PacketMsg& packet)
{
    try {
        auto response =
            client.crypto().generate_encryption_response(packet);
        client.send_msg(response);
    } catch (const std::exception& e) {
        client.emit(Events::ChannelSecuredEvent{
            Events::EventResult::fail(e.what())
        });
    }
}

static void handle_encrypt_result(
    CMClient& client,
    const Packets::PacketMsg& packet)
{
    ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResult> res(packet);

    bool success = res.Body.result == Enums::EResult::OK;
    client.set_channel_secured(success);

    Events::ChannelSecuredEvent evt{
        success
            ? Events::EventResult::ok()
            : Events::EventResult::fail("Failed to encrypt channel; bad server?")
    };

    client.emit_event(evt);
}

static Steam::Dispatch::MsgRegister<
    Enums::EMsg::ChannelEncryptRequest,
    handle_encrypt_request
> reg_encrypt;

static Steam::Dispatch::MsgRegister<
    Enums::EMsg::ChannelEncryptResult,
    handle_encrypt_result
> reg_result;
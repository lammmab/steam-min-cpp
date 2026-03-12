#include "dispatch/register.hpp"
#include "network/cmclient.hpp"

using namespace Steam;
using namespace Steam::Messaging;

static void handle_encrypt_request(
    CMClient& client,
    const Packets::PacketMsg& packet)
{
    try {
        auto response =
            client.crypto().generate_encryption_response(packet);
        client.send_msg(response);
    } catch (const std::exception& e) {
        client.emit(Steam::Events::ChannelSecuredEvent{
            Steam::Events::EventResult{
                success: false,
                what: e.what()
            }
        });
    }
}

static void handle_encrypt_response(
    CMClient& client,
    const Packets::PacketMsg& packet)
{
    ClientMessages::Msg<Steam::Internal::MsgChannelEncryptResult> res(packet);
    client.emit(Steam::Events::ChannelSecuredEvent{
        Steam::Events::EventResult {
            success: res.Body.result == Steam::Internal::Enums::EResult::OK
        }
    });
}

static Steam::Dispatch::MsgRegister<
    Steam::Internal::Enums::EMsg::ChannelEncryptRequest,
    handle_encrypt_request
> reg_encrypt;

static Steam::Dispatch::MsgRegister<
    Steam::Internal::Enums::EMsg::ChannelEncryptResult,
    handle_encrypt_response
> reg_response;
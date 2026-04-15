#include "network/cmclient.hpp"
#include "utils/macros.h"

#include <exception>

STEAMCLIENT_FILE_LOGGER();

using namespace Steam::Messaging;

// Connect via TCP, then start message loop
void CMClient::start_session() {
    try {
        connection_->set_on_frame([this](const std::vector<uint8_t>& frame) {
            consume_frame(frame);
        });

        connection_->network_connect();
    } catch (const std::exception& e) {
        STEAMCLIENT_LOG_ERROR("Could not connect via TCP: {}",e.what());
        throw;
    }
}

// Start consuming TCP messages
void CMClient::consume_frame(const std::vector<uint8_t>& frame, bool encrypt) {
    if (frame.size() < 4) return;
    std::vector<uint8_t> data = frame;

    if (encrypt && channel_secured_) {
        data = crypto_.process_incoming_encrypted_message(frame);
    }

    uint32_t emsg = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    try {
        if (is_encryption_msg(emsg)) {
            // We're a raw Msg
            Packets::PacketMsg raw(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            Steam::Dispatch::dispatch_msg(*this,raw);
        } else if (Steam::Utils::MsgUtil::is_protobuf_msg(emsg)) {
            // We're a protobuf Msg
            Packets::PacketClientMsgProtobuf proto(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            Steam::Dispatch::dispatch_proto(*this, proto);
        } else {
            // We're a struct msg
            /*logger->info("Received struct Msg");
            auto str   = std::make_unique<Packets::PacketClientMsg>(
                static_cast<Steam::Internal::Enums::EMsg>(emsg), data
            );
            emit(*str);*/
        }

    } catch (const std::exception& e) {
        STEAMCLIENT_LOG_ERROR("Failed to parse frame (EMsg: {}): {}", emsg, e.what());
    }
}

void CMClient::send_msg_impl(std::vector<byte> buffer)
{
    if (channel_secured_) {
        buffer = crypto_.process_outgoing_encrypted_message(buffer);
    }
    connection_->async_send(buffer);
}

// Shuts down the internal CMClient
void CMClient::shutdown() {
    if (connection_ && connection_->is_connected())
        connection_->network_close();
    
    STEAMCLIENT_LOG_INFO("Closed connection and CMClient successfully");
}

void CMClient::on_erased(std::type_index type, std::function<void(const void*)> handler) {
    erased_listeners_[type] = std::move(handler);
}

void CMClient::emit_erased(std::type_index type, const void* evt) {
    auto it = erased_listeners_.find(type);
    if (it != erased_listeners_.end())
        it->second(evt);
}

template<typename Type>
void CMClient::emit_event(const Type& evt) {
    emit(evt);
    emit_erased(std::type_index(typeid(evt)), &evt);
}
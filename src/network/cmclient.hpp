#pragma once

#include "events/event_emitter.h"
#include "events/events.h"

#include "network/connection/connection.hpp"
#include <string>
#include <memory>
#include <exception>

#include <cstdint>
#include <vector>

#include <atomic>
#include <thread>

#include "base/packetbase.hpp"
#include "base/msgbase.hpp"

#include "crypto/crypto.hpp"

namespace Steam::Messaging {
    class CMClient: public medooze::EventEmitter {
    public:
        CMClient(std::unique_ptr<Steam::Networking::IConnection> connection)
            : connection_(std::move(connection))
        {}

        ~CMClient() 
        {
            if (connection_ && connection_->is_connected()) connection_->network_close();
        }

        void start_session();
        void shutdown();
        inline bool is_connected() const {
            return connection_->is_connected();
        }

        inline bool is_channel_secured() const {
            return channel_secured_;
        }

        // This solution is super hacky; we will 100% want to correct this later and fix the base classes.
        template<typename THdr>
        inline void send_msg(const Messages::MsgBaseHdr<THdr>& msg)
        {
            send_msg_impl(msg.Serialize());
        }

        void send_msg_impl(std::vector<byte> buffer);
        void parse_multi(const Packets::PacketClientMsgProtobuf& msg);

    private:

        void consume_frame(const std::vector<uint8_t>& frame, bool encrypt = true);
        inline const bool is_encryption_msg(uint32_t emsg_code) {
            return (
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptRequest ||
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResponse ||
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResult
            );
        }
        
        void rcv_msg_proto(const Steam::Messaging::Packets::PacketClientMsgProtobuf& msg);
        void rcv_msg(const Steam::Messaging::Packets::PacketMsg& msg);
        // Add handling for struct messages

        void setup_handlers();

        bool channel_secured_ = false;
        std::unique_ptr<Steam::Networking::IConnection> connection_;  
        Steam::Crypto::EncryptionManager crypto_;
    };
}
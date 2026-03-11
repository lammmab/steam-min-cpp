#pragma once
#include "utils/event_emitter.h"
#include "network/connection.hpp"
#include <string>
#include <memory>
#include <exception>

#include <spdlog/spdlog.h>

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
        {
            spdlog::info("Setup CMClient");
        }

        ~CMClient() 
        {
            if (connection_ && connection_->is_connected()) connection_->network_close();
        }

        void start_session();
        void shutdown();
        inline bool is_connected() const {
            return connection_->is_connected();
        }
        
    private:

        void consume_frame(const std::vector<uint8_t>& frame);
        inline const bool is_encryption_msg(uint32_t emsg_code) {
            return (
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptRequest ||
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResponse ||
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResult
            );
        }

        
        template<typename TBody>
        void send_msg(const Steam::Messaging::ClientMessages::Msg<TBody>& msg);
        

        void rcv_msg_proto(const Steam::Messaging::Packets::PacketClientMsgProtobuf& msg);
        void rcv_msg(const Steam::Messaging::Packets::PacketMsg& msg);
        // Add handling for struct messages

        void setup_handlers();

        bool channel_secured_ = false;
        std::unique_ptr<Steam::Networking::IConnection> connection_;  
        Steam::Crypto::EncryptionManager crypto_;
    };
}
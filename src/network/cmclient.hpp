#include "utils/event_emitter.h"
#include "network/tcp.hpp"
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
        CMClient(asio::io_context& ctx)
            : connection_(std::make_unique<Steam::Networking::TCPConnection>(ctx))
        {
            spdlog::info("Setup CMClient");
        }

        ~CMClient() 
        {
            if (connection_ && connection_->is_connected()) connection_->close_tcp();
        }

        void start_session();
        void shutdown();
        inline bool is_tcp_connected() const {
            return connection_->is_connected();
        }
        
    private:

        void consume_frame(std::vector<uint8_t> frame);
        inline bool is_protobuf_msg(uint32_t emsg_code) {
            if (
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptRequest ||
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResponse ||
                emsg_code == (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResult
            ) {
                return false;
            }

            return true;
        }

        
        //void send_msg_proto(const MsgProto& msg);
        template<typename TBody>
        void send_msg(const Steam::Messaging::ClientMessages::Msg<TBody>& msg);
        

        void rcv_msg_proto(const Steam::Messaging::Packets::PacketClientMsgProtobuf& msg);
        void rcv_msg(const Steam::Messaging::Packets::PacketMsg& msg);


        void setup_handlers();

        bool channel_secured_ = false;
        std::unique_ptr<Steam::Networking::TCPConnection> connection_;  
        Steam::Crypto::EncryptionManager crypto_;
    };
}
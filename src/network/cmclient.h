#include "utils/event_emitter.h"
#include "network/connection.h"
#include <string>
#include <memory>
#include <exception>
#include <spdlog/spdlog.h>
#include <cstdint>
#include <vector>

#include <atomic>
#include <thread>

#include "base/packetbase.h"
#include "base/msgbase.h"

#include "network/tasks/encryption.h"

class CMClient: public medooze::EventEmitter {
public:
    CMClient(asio::io_context& ctx)
        : connection_(std::make_unique<TCPConnection>(ctx))
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
            emsg_code == (uint32_t)SteamInternal::EMsg::ChannelEncryptRequest ||
            emsg_code == (uint32_t)SteamInternal::EMsg::ChannelEncryptResponse ||
            emsg_code == (uint32_t)SteamInternal::EMsg::ChannelEncryptResult
        ) {
            return false;
        }

        return true;
    }

    
    //void send_msg_proto(const MsgProto& msg);
    template<typename TBody>
    void send_msg(const Msg<TBody>& msg);
    

    void rcv_msg_proto(const PacketClientMsgProtobuf& msg);
    void rcv_msg(const PacketMsg& msg);


    void setup_handlers();

    bool channel_secured_ = false;
    std::unique_ptr<TCPConnection> connection_;
};
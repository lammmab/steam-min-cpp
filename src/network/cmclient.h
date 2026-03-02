#include "utils/event_emitter.h"
#include "network/connection.h"
#include <string>
#include <memory>
#include <exception>
#include <spdlog/spdlog.h>
#include <cstdint>
#include <vector>

#include "network/msg/msgproto.h"
#include "network/msg/extendedmsg.h"
#include "network/msg/msg.h"

#include "network/msg/typed/typedmsg.h"

#include "network/tasks/encryption.h"

#include <atomic>
#include <thread>

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
    
    std::vector<uint8_t> channel_key;
    std::vector<uint8_t> channel_hmac;
    const size_t header_size = 8;

private:

    void consume_frame(std::vector<uint8_t> frame);

    void send_msg_proto(const MsgProto& msg);
    void send_msg(const Msg& msg);


    void rcv_msg_proto(const MsgProto& msg);
    void rcv_msg(const Msg& msg);

    void setup_handlers();

    bool channel_secured_ = false;
    std::unique_ptr<TCPConnection> connection_;
    SteamEncryptionHandler encryption_;
};
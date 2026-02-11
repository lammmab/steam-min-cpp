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

#include <atomic>
#include <thread>
#include "crypto/crypto.h"
class CMClient: public medooze::EventEmitter {
public:
    enum class EventMessage {
        CONNECTED,
        DISCONNECTED,
        RECONNECT,
        CHANNEL_SECURED,
        ERR,
        EMSG
    };

    CMClient(asio::io_context& ctx)
        : connection_(std::make_unique<TCPConnection>(ctx))
    {
        spdlog::info("Setup CMClient hooks");
    }

    ~CMClient() { if (connection_->is_connected()) connection_->close(); }

    void start_session();
    void shutdown();
    bool is_tcp_connected();
    
    std::vector<uint8_t> channel_key;
    std::vector<uint8_t> channel_hmac;
    const size_t header_size = 8;

    private:

    void start_message_loop();
    void handle_channel_encrypt_request(const ExtendedMsgHdr& req);
    void rcv_msg(const MsgProto& msg); 
    void rcv_msg(const Msg& msg); 

    void send_msg(const MsgProto& msg);

    std::thread msg_thread_;
    std::atomic<bool> stop_msg_thread_{false};
    bool channel_secured_ = false;
    std::unique_ptr<TCPConnection> connection_;
    SteamCrypto crypto_;
};
/// @internal
/// \cond

#pragma once

#include <steamclient/external/event_emitter.h>

#include <boost/asio/steady_timer.hpp>
#include <cstdint>
#include <memory>
#include <steamclient/connections/connection.hpp>
#include <steamclient/crypto/crypto.hpp>
#include <steamclient/routing/commands/router.hpp>
#include <steamclient/routing/events/dispatcher.hpp>
#include <steamclient/types/msgbase.hpp>
#include <steamclient/types/packetbase.hpp>
#include <vector>

namespace Steam::Messaging {
class JobIDs {
  using Job = std::function<void(const Steam::Messaging::Packets::IPacketMsg&)>;

 public:
  static uint64_t generate_jobid() { return next_jobid_++; }

 private:
  inline static uint64_t next_jobid_ = 0;
  inline static std::unordered_map<uint64_t, Job> pending_jobs_;
};

class CMClient : public medooze::EventEmitter {
 public:
  CMClient(std::unique_ptr<Steam::Networking::IConnection> connection,
           boost::asio::io_context& ctx)
      : io_ctx_(ctx), connection_(std::move(connection)) {}

  ~CMClient() { shutdown(); }

  void start_session();
  void shutdown();
  inline bool is_connected() const { return connection_->is_connected(); }

  inline bool is_channel_secured() const { return channel_secured_; }

  inline void set_channel_secured(bool val) { channel_secured_ = val; }

  inline Steam::Crypto::EncryptionManager& crypto() { return crypto_; }

  template <typename Request>
  inline void execute(const Request& req) {
    using namespace Steam::Dispatch;

    size_t id = Steam::Dispatch::request_id<Request>();
    auto fn = Steam::Dispatch::g_request_router.table[id];

    if (fn) fn(*this, &req);
  }

  // Send a message to the connected Steam server.
  template <typename THdr>
  inline void send_msg(const Messages::MsgBaseHdr<THdr>& msg) {
    send_msg_impl(msg.Serialize());
  }

  void consume_frame(const std::vector<uint8_t>& frame, bool encrypt = true);
  void kickoff_heartbeat(int interval);

 private:
  void schedule_heartbeat();
  void send_msg_impl(std::vector<byte> buffer);
  void reset();

  inline const bool is_encryption_msg(uint32_t emsg_code) {
    return (
        emsg_code ==
            (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptRequest ||
        emsg_code ==
            (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResponse ||
        emsg_code ==
            (uint32_t)Steam::Internal::Enums::EMsg::ChannelEncryptResult);
  }

  bool channel_secured_ = false;
  boost::asio::io_context& io_ctx_;
  std::unique_ptr<Steam::Networking::IConnection> connection_;
  Steam::Crypto::EncryptionManager crypto_;
  std::optional<boost::asio::steady_timer> heartbeat_timer_;
  std::chrono::seconds heartbeat_interval_{};
};
}  // namespace Steam::Messaging
/// \endcond
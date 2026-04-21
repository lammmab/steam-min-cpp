/// @internal
#pragma once

#include <steamclient/external/event_emitter.h>

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
  CMClient(std::unique_ptr<Steam::Networking::IConnection> connection)
      : connection_(std::move(connection)) {}

  ~CMClient() {
    if (connection_ && connection_->is_connected())
      connection_->network_close();
  }

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

  // This solution is super hacky; we will 100% want to correct this later and
  // fix the base classes.
  template <typename THdr>
  inline void send_msg(const Messages::MsgBaseHdr<THdr>& msg) {
    send_msg_impl(msg.Serialize());
  }

  void consume_frame(const std::vector<uint8_t>& frame, bool encrypt = true);

 private:
  void send_msg_impl(std::vector<byte> buffer);

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
  std::unique_ptr<Steam::Networking::IConnection> connection_;
  Steam::Crypto::EncryptionManager crypto_;
};
}  // namespace Steam::Messaging
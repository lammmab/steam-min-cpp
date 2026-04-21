/// @file events.hpp
///
/// Event and command definitions used by the Steam messaging layer.
///
/// Architecture:
/// - Commands represent requests sent to the Steam CM server.
/// - Events represent asynchronous responses or notifications.
/// - Commands are delivered through the `SteamClient::execute` dispatch system.
/// - Events are delivered through the `SteamClient::on` callback system.

#pragma once

#include <cstdint>
#include <steamclient/utilities/vdf.hpp>
#include <string>


namespace Steam::Messaging::Packets {
class IPacketMsg;
}

namespace Steam::Events {

/// Base type for all emitted events.
///
/// All events dispatched by the internal messaging system derive
/// from this type.
struct Event {
  virtual ~Event() = default;
};

/// Result information returned by operations.
///
/// Convention:
/// - `success == true` indicates the request completed successfully.
/// - `message` contains a diagnostic message only when `success == false`.
struct EventResult {
  /// Indicates whether the operation succeeded.
  bool success;

  /// Optional error or diagnostic message.
  std::string message;

  /// Construct a successful result.
  static EventResult ok() { return {true, {}}; }

  /// Construct a failure result.
  /// @param msg Failure description.
  static EventResult fail(std::string msg) { return {false, std::move(msg)}; }
};

/// Event containing an operation result.
///
/// Many Steam responses are represented as a result status with
/// optional metadata. This event type is commonly used as a base
/// for more specific response events.
struct ResultEvent : Event {
  /// Result of the associated operation.
  EventResult result;

  ResultEvent() = default;
  ResultEvent(EventResult r) : result(std::move(r)) {}

  /// Check whether the operation succeeded.
  bool ok() const { return result.success; }

  /// Retrieve the failure message if present.
  const std::string& what() const { return result.message; }
};

/// Emitted when the channel encryption handshake completes.
///
/// Indicates whether the secure channel with the Steam CM server
/// was successfully established.
struct ChannelSecuredEvent : ResultEvent {
  using ResultEvent::ResultEvent;
};

/// Result of a client logon attempt.
///
/// Emitted after the client attempts to authenticate with the
/// Steam CM server.
struct ClientLogonEvent : ResultEvent {
  using ResultEvent::ResultEvent;

  /// Authenticated SteamID for the session.
  uint64_t steamid;

  /// Steam CM session identifier.
  int32_t client_sessionid;

  ClientLogonEvent(EventResult r, uint64_t s_id, int32_t c_sid)
      : ResultEvent(std::move(r)), steamid(s_id), client_sessionid(c_sid) {}
};

/// Emitted when a message type is not implemented by the client.
///
/// Useful for debugging unsupported Steam message types.
struct MsgNotImplementedEvent : Event {
  /// Numeric Steam message identifier.
  uint32_t emsg;

  /// Raw packet associated with the message.
  ///
  /// Provided for debugging or custom handling. Most applications
  /// can safely ignore this pointer.
  const Steam::Messaging::Packets::IPacketMsg* packet;

  explicit MsgNotImplementedEvent(
      uint32_t e, const Steam::Messaging::Packets::IPacketMsg* p)
      : emsg(e), packet(p) {}
};

/// Result of a product information request.
///
/// Returned in response to a `GetProductInfo` command.
struct ProductInfoResult : Event {
  /// Application information entry.
  struct ProductAppInfo {
    /// Application identifier.
    uint32_t appid;

    /// SHA hash representing the appinfo version.
    std::string sha;

    /// Parsed VDF metadata for the application.
    Steam::Utils::VDF::VDFNode appinfo;
  };

  /// Package information entry.
  struct ProductPackageInfo {
    /// Package identifier.
    uint32_t packageid;

    /// SHA hash representing the package version.
    std::string sha;

    /// Parsed VDF metadata for the package.
    Steam::Utils::VDF::VDFNode pkginfo;
  };

  /// Returned application metadata.
  std::vector<ProductAppInfo> apps;

  /// Returned package metadata.
  std::vector<ProductPackageInfo> packages;

  explicit ProductInfoResult(std::vector<ProductAppInfo> a,
                             std::vector<ProductPackageInfo> p)
      : apps(std::move(a)), packages(std::move(p)) {}
};

}  // namespace Steam::Events

namespace Steam::Commands {

/// Base type for commands sent to the Steam CM server.
struct Command {
  virtual ~Command() = default;
};

/// Request an anonymous Steam login.
///
/// Used to establish a session without user credentials.
struct AnonymousLogin : Command {};

/// Request termination of the current Steam user session.
struct ClientLogoff : Command {};

/// Request product information for a Steam application.
///
/// Requires an authenticated CM session. The `steamid` and `client_sessionid`
/// fields must match the values returned by `ClientLogonEvent`.
struct GetProductInfo : Command {
  /// Target application ID.
  uint32_t id;

  /// SteamID associated with the current session.
  uint64_t steamid;

  /// Steam CM session identifier.
  int32_t client_sessionid;

  /// Optional application access token.
  ///
  /// Providing a token allows access to additional restricted
  /// metadata when available.
  uint64_t app_token = 0;

  /// Construct a request without an app token.
  explicit GetProductInfo(uint32_t i, uint64_t s_id, int32_t c_sid)
      : id(i), steamid(s_id), client_sessionid(c_sid) {}

  /// Construct a request with an app token.
  explicit GetProductInfo(uint32_t i, uint64_t s_id, int32_t c_sid, uint64_t t)
      : id(i), steamid(s_id), client_sessionid(c_sid), app_token(t) {}
};
}  // namespace Steam::Commands
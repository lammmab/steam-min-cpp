#include <steamclient/macros.h>

#include <boost/asio.hpp>
#include <cstdio>
#include <iostream>
#include <steamclient/client.hpp>
#include <steamclient/connections/tcp.hpp>
#include <steamclient/events.hpp>
#include <steamclient/utilities/vdf.hpp>
#include <thread>

STEAMCLIENT_FILE_LOGGER();

namespace Events = Steam::Events;
namespace Commands = Steam::Commands;
namespace VDF = Steam::Utils::VDF;

int main() {
  boost::asio::io_context io_ctx;
  auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx);

  Steam::SteamClient client(std::move(connection), io_ctx);

  client.on<Events::MsgNotImplementedEvent>(
      [](const Events::MsgNotImplementedEvent& e) {
        STEAMCLIENT_LOG_INFO("Msg with id {} is not implemented.", e.emsg);
      });

  client.on<Events::ProductInfoResult>(
      [](const Events::ProductInfoResult& res) {
        STEAMCLIENT_LOG_INFO("Received ProductInfoResult\n");
        STEAMCLIENT_LOG_INFO("Apps: {}", res.apps.size());
        STEAMCLIENT_LOG_INFO("Packages: {}", res.packages.size());

        for (const auto& app : res.apps) {
          STEAMCLIENT_LOG_INFO("AppID: {} | SHA: {}", app.appid, app.sha);

          try {
            if (auto common = app.appinfo.get("common")) {
              auto name = common->get_string_or("name", "Unknown");
              STEAMCLIENT_LOG_INFO("Name: {}", name);
            }
          } catch (...) {
            STEAMCLIENT_LOG_ERROR("<not found>");
          }
        }

        for (const auto& pkg : res.packages) {
          STEAMCLIENT_LOG_INFO("PackageID: {} | SHA: {}", pkg.packageid,
                               pkg.sha);
        }
      });

  client.on<Events::ClientLogonEvent>(
      [&client](const Events::ClientLogonEvent& res) {
        if (res.ok()) {
          uint32_t app = 440;
          STEAMCLIENT_LOG_INFO("Successfully logged on");
          STEAMCLIENT_LOG_INFO(
              "Attempting to get information about this app ID: {}", app);

          client.execute(
              Commands::GetProductInfo{app, res.steamid, res.client_sessionid});
        } else {
          STEAMCLIENT_LOG_ERROR("Logon failed: {}", res.what());
        }
      });

  client.on<Events::ChannelSecuredEvent>(
      [&client](const Events::ChannelSecuredEvent& res) {
        if (res.ok()) {
          STEAMCLIENT_LOG_INFO("Successfully secured channel");
          client.execute(Commands::AnonymousLogin{});
        } else {
          STEAMCLIENT_LOG_ERROR("Failed to secure channel :(");
        }
      });

  client.connect();

  std::thread io_thread([&io_ctx]() { io_ctx.run(); });

  std::cin.get();

  client.disconnect();

  io_ctx.stop();
  io_thread.join();

  return 0;
}
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

  client.on<Events::ClientLogonEvent>(
      [&client](const Events::ClientLogonEvent& res) {
        if (res.ok()) {
          STEAMCLIENT_LOG_INFO("Successfully logged on");
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
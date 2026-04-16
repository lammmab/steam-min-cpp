#include <iostream>
#include <cstdio>
#include <thread>
#include <boost/asio.hpp>
#include <steamclient/client.hpp>
#include <steamclient/connections/tcp.hpp>
#include <steamclient/events.hpp>
#include <steamclient/vdf.hpp>

namespace Events = Steam::Events;
namespace Commands = Steam::Commands;
namespace VDF = Steam::Utils::VDF;

int main() {
    boost::asio::io_context io_ctx;
    auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx);

    Steam::SteamClient client(std::move(connection));

    client.on<Events::ClientLogonEvent>(
        [&client](const Events::ClientLogonEvent& res) {
            if (res.ok()) {
                printf("Successfully logged on\n");
            } else {
                printf("Logon failed: %s\n", res.what());
            }
        }
    );

    client.on<Events::ChannelSecuredEvent>(
        [&client](const Events::ChannelSecuredEvent& res) {
            if (res.ok()) {
                printf("Successfully secured channel\n");
                client.execute(Commands::AnonymousLogin{});
            } else {
                printf("Failed to secure channel :(\n");
            }
        }
    );

    client.connect();

    std::thread io_thread([&io_ctx]() {
        io_ctx.run();
    });

    std::cin.get();

    client.disconnect();

    io_ctx.stop();
    io_thread.join();

    return 0;
}
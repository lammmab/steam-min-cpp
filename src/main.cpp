#include <iostream>
#include <boost/asio.hpp>
#include "client/client.hpp"
#include "network/connection/tcp.hpp"

#include "utils/macros.h"

FILE_LOGGER();

namespace Events = Steam::Events;
namespace Commands = Steam::Commands;

int main() {
    boost::asio::io_context io_ctx;
    auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx);

    Steam::SteamClient client(std::move(connection));

    client.on<Events::ClientLogonEvent>([](const Events::ClientLogonEvent& res) {
        if (res.ok()) {
            logger->info("Successfully logged on");
        } else {
            logger->info(res.what());
        }
    });

    client.on<Events::ChannelSecuredEvent>([&client](const Events::ChannelSecuredEvent& res) {
        if (res.ok()) {
            logger->info("Sucessfully secured channel");
            client.execute(Commands::AnonymousLogin{});
        }
    });
    
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


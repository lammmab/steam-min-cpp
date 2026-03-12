#include <iostream>
#include <boost/asio.hpp>
#include "client/client.hpp"
#include "network/connection/tcp.hpp"

#include "events/events.h"

#include "utils/macros.h"

FILE_LOGGER();

int main() {
    boost::asio::io_context io_ctx;
    auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx);

    Steam::SteamClient client(std::move(connection));

    client.connect();
    std::thread io_thread([&io_ctx]() {
        io_ctx.run();
    });

    client.client_on<Steam::Events::ChannelSecuredEvent>([&client](const Steam::Events::ChannelSecuredEvent&) {
        logger->info("Sending anonymous login");
        if (client.anonymous_login()) {
            ;
        } else {
            logger->info("Login failed");
        }
    });

    std::cin.get();

    client.disconnect();

    io_ctx.stop();
    io_thread.join();

    return 0;
}


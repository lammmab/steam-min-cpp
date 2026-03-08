#include <iostream>
#include <boost/asio.hpp>
#include "client/client.hpp"
#include "network/tcp.hpp"

int main() {
    boost::asio::io_context io_ctx;
    auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx);

    Steam::SteamClient client(std::move(connection));

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


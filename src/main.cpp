#include <iostream>
#include <asio.hpp>
#include "client/client.h"

int main() {
    asio::io_context io_ctx;
    
    SteamClient client(io_ctx);

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


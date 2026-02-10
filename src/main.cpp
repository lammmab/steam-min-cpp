#include <iostream>
#include <asio.hpp>
#include "client/client.h"

int main() {
    asio::io_context io_ctx;

    
    std::thread io_thread([&io_ctx]() {
        io_ctx.run();
    });
    
    SteamClient client(io_ctx);

    client.connect();

    std::cout << "Press Enter to disconnect...\n";
    std::cin.get();

    client.disconnect();

    io_ctx.stop();
    io_thread.join();
    std::cout << "Press Enter to stop...\n";

    return 0;
}


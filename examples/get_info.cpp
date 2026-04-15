#include <steamclient/client.hpp>
#include <steamclient/connections/tcp.hpp>
#include <steamclient/events.hpp>

namespace Events = Steam::Events;
namespace Commands = Steam::Commands;

int main() {
    boost::asio::io_context io_ctx; // Create the Asio context for low-level TCP work
    auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx); // Make a unique connection to the TCPConnection

    Steam::SteamClient client(std::move(connection)); // Pass ownership of the connection and create a SteamClient

    std::thread io_thread([&io_ctx]() {
        io_ctx.run(); // Run the io thread
    });

    client.connect(); // Connect to the TCP server / kick off the encryption phase

    client.on<Events::ProductInfoResult>([&client](const Events::ProductInfoResult& res) {
        std::cout << res.apps[0].appid; // Connect to the ProductInfoResult and output the result.
        client.disconnect(); // Close the connection.
    });

    client.on<Events::ClientLogonEvent>([&client](const Events::ClientLogonEvent& res) {
        if (res.ok()) {
            client.execute(
                Commands::GetProductInfo{
                    440, // TF2 
                    res.steamid, 
                    res.client_sessionid
                }
            );
        }
    });

    client.on<Events::ChannelSecuredEvent>([&client](const Events::ChannelSecuredEvent& res) {
        if (res.ok()) {
            client.execute(Commands::AnonymousLogin{});
        }
    }); // Connect to the encryption result and send an anonymous login

}
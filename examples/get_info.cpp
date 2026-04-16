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

    client.on<Events::MsgNotImplementedEvent>(
        [](const Events::MsgNotImplementedEvent& e) {
            printf("Msg with id %u is not implemented.\n", e.emsg);
        }
    );

    client.on<Events::ProductInfoResult>(
        [](const Events::ProductInfoResult& res) {
            printf("Received ProductInfoResult\n");
            printf("Apps: %zu\n", res.apps.size());
            printf("Packages: %zu\n", res.packages.size());

            for (const auto& app : res.apps) {
                printf("AppID: %u | SHA: %s\n", app.appid, app.sha.c_str());

                try {
                    if (auto common = app.appinfo.get("common")) {
                        auto name = common->get_string_or("name", "Unknown");
                        printf("Name: %s\n", name.c_str());
                    }
                } catch (...) {
                    printf("<not found>\n");
                }
            }

            for (const auto& pkg : res.packages) {
                printf("PackageID: %u | SHA: %s\n", pkg.packageid, pkg.sha.c_str());
            }
        }
    );

    client.on<Events::ClientLogonEvent>(
        [&client](const Events::ClientLogonEvent& res) {
            if (res.ok()) {
                uint32_t app = 440;
                printf("Successfully logged on\n");
                printf("Attempting to get information about this app ID: %u\n", app);

                client.execute(Commands::GetProductInfo{
                    app,
                    res.steamid,
                    res.client_sessionid
                });
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
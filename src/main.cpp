#include <iostream>
#include <boost/asio.hpp>
#include "client/client.hpp"
#include "network/connection/tcp.hpp"

#include "utils/macros.h"
#include "utils/vdf/vdf.hpp"
FILE_LOGGER();

namespace Events = Steam::Events;
namespace Commands = Steam::Commands;
namespace VDF = Steam::Utils::VDF;

int main() {
    boost::asio::io_context io_ctx;
    auto connection = std::make_unique<Steam::Networking::TCPConnection>(io_ctx);

    Steam::SteamClient client(std::move(connection));

    client.on<Events::MsgNotImplementedEvent>([&client](const Events::MsgNotImplementedEvent& e) {
        logger->info("Msg with id {} is not implemented.",e.emsg);
    });

    client.on<Events::ProductInfoResult>([](const Events::ProductInfoResult& res) {
        logger->info("Received ProductInfoResult");
        logger->info("Apps: {}", res.apps.size());
        logger->info("Packages: {}", res.packages.size());

        for (const auto& app : res.apps)
        {
            logger->info("AppID: {} | SHA: {}", app.appid, app.sha);

            try {
                if (auto common = app.appinfo.get("common")) {
                    auto name = common->get_string_or("name", "Unknown");
                    logger->info("Name: {}", name);
                }
            }
            catch (...) {
                logger->info("<not found>");
            }
        }

        for (const auto& pkg : res.packages)
        {
            logger->info("PackageID: {} | SHA: {}", pkg.packageid, pkg.sha);
        }
    });

    client.on<Events::ClientLogonEvent>([&client](const Events::ClientLogonEvent& res) {
        if (res.ok()) {
            uint32_t app = 440;
            logger->info("Successfully logged on");
            logger->info("Attempting to get information about this app ID: {}",app);
            client.execute(Commands::GetProductInfo{app,res.steamid,res.client_sessionid});
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


#include "dispatch/register.hpp"
#include "network/cmclient.hpp"

#include "protogen/steammessages_clientserver_appinfo.pb.h"
#include "utils/vdf/vdf.hpp"

using namespace Steam;
using namespace Messaging;
using namespace Utils;

std::string hexlify(const char* data, size_t len)
{
    static const char* hex = "0123456789abcdef";

    std::string out;
    out.reserve(len * 2);

    for (size_t i = 0; i < len; ++i)
    {
        uint8_t b = static_cast<uint8_t>(data[i]);
        out.push_back(hex[b >> 4]);
        out.push_back(hex[b & 0xF]);
    }

    return out;
}

static Events::ProductInfoResult::ProductAppInfo parse_app(const auto& app)
{
    std::string buffer = app.buffer();
    if (!buffer.empty())
        buffer.pop_back();

    auto vdf = VDF::parse_text(buffer);

    return {
        app.appid(),
        hexlify(app.sha().data(), app.sha().size()),
        vdf["appinfo"].as_object()
    };
}

static Events::ProductInfoResult::ProductPackageInfo parse_pkg(const auto& pkg)
{
    const std::string& buffer = pkg.buffer();

    auto vdf = VDF::parse_binary(
        reinterpret_cast<const uint8_t*>(buffer.data()) + 4,
        buffer.size() - 4
    );

    return {
        pkg.packageid(),
        hexlify(pkg.sha().data(), pkg.sha().size()),
        vdf[std::to_string(pkg.packageid())].as_object()
    };
}

static void handle_prod_info_response(
    CMClient& client,
    const Packets::PacketClientMsgProtobuf& packet)
{
    ClientMessages::MsgProto<CMsgClientPICSProductInfoResponse> msg(packet);

    std::vector<Events::ProductInfoResult::ProductAppInfo> apps;
    std::vector<Events::ProductInfoResult::ProductPackageInfo> packages;

    for (const auto& app : msg.Body.apps())
        apps.push_back(parse_app(app));

    for (const auto& pkg : msg.Body.packages())
        packages.push_back(parse_pkg(pkg));

    client.emit(Events::ProductInfoResult{
        std::move(apps),
        std::move(packages)
    });
}

static Steam::Dispatch::ProtoRegister<
    Internal::Enums::EMsg::ClientPICSProductInfoResponse,
    handle_prod_info_response
> reg_prod_info;
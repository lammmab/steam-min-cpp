#include "dispatch/register.hpp"
#include "network/cmclient.hpp"

#include "steammessages_clientserver_login.pb.h"
#include "base/clientmsg.hpp"

#include "utils/gzip/gzip_helpers.hpp"
#include <boost/endian/conversion.hpp>

using namespace Steam;
using namespace Steam::Messaging;

bool read_frame(
    const std::vector<uint8_t>& payload,
    size_t& offset,
    std::vector<uint8_t>& out)
{
    if (offset + sizeof(uint32_t) > payload.size())
        return false;

    uint32_t subSize;
    memcpy(&subSize, payload.data() + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    subSize = boost::endian::native_to_little(subSize);

    if (offset + subSize > payload.size())
        return false;

    out.assign(
        payload.begin() + offset,
        payload.begin() + offset + subSize
    );

    offset += subSize;
    return true;
}

static void handle_multi(
    CMClient& client,
    const Packets::PacketClientMsgProtobuf& packet)
{
    ClientMessages::MsgProto<CMsgMulti> multi(packet);

    std::vector<uint8_t> payload(
        multi.Body.message_body().begin(),
        multi.Body.message_body().end());

    if (multi.Body.size_unzipped() > 0)
        payload = Steam::Utils::GZip::gzip_decompress(
            payload,
            multi.Body.size_unzipped());

    size_t offset = 0;
    std::vector<uint8_t> frame;

    while (read_frame(payload, offset, frame))
        client.consume_frame(frame, false);
}

static Steam::Dispatch::ProtoRegister<
    Steam::Internal::Enums::EMsg::Multi,
    handle_multi
> reg_multi;
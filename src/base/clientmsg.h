// GPT
#include "base/msgbase.h"
#include "base/generated/Stream.h"

template <typename TBody>
class Msg : public MsgBaseHdr<SteamInternal::Internal::MsgHdr>
{
    static_assert(std::is_base_of<SteamInternal::Internal::ISteamSerializableMessage, TBody>::value,
                  "TBody must implement ISteamSerializableMessage");

public:
    TBody Body;
    explicit Msg(int payloadReserve = 0)
    {
        Payload().reserve(payloadReserve);

        Header.SetEMsg(Body.GetEMsg());
    }

    Msg(const Msg<TBody>& other, int payloadReserve = 0)
        : Msg(payloadReserve)
    {
        Header.targetJobID = other.Header.sourceJobID;
    }

    explicit Msg(const IPacketMsg& packet)
    {
        const PacketMsg* packetMsg = dynamic_cast<const PacketMsg*>(&packet);
        if (!packetMsg)
            throw std::runtime_error("Msg<TBody> used for wrong packet type");

        Header = packetMsg->header;

        const std::vector<byte>& data = packetMsg->GetData();
        size_t offset = packetMsg->bodyOffset;

        Stream stream(data,offset, StreamingMode::Read);

        Body.Deserialize(stream);

        if (offset < data.size())
        {
            Payload().insert(Payload().end(),
                           data.begin() + offset,
                           data.end());
        }
    }

public:
    bool IsProto() const override
    {
        return false;
    }

    SteamInternal::EMsg MsgType() const override
    {
        return Header.Msg;
    }

    std::vector<byte> Serialize() const override 
    {
        spdlog::info("Serializing this EMsg: {}",static_cast<uint32_t>(MsgType()));
        Stream stream(StreamingMode::Write);

        Header.Serialize(stream);
        spdlog::info("Serialized header, now {} bytes",stream.Length());
        Body.Serialize(stream);
        spdlog::info("Serialized body, now {} bytes",stream.Length());
        stream.Write(Payload().data(), Payload().size());
        spdlog::info("Payload length: {}", Payload().size());
        spdlog::info("Serialized final payload, now {} bytes",stream.Length());

        return stream.MoveBuffer();
    }
};
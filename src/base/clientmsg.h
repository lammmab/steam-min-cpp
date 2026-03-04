#include "base/msgbase.h"
#include "base/generated/Stream.h"

template <typename TBody>
class Msg : public MsgBase
{
    static_assert(std::is_base_of<ISteamSerializableMessage, TBody>::value,
                  "TBody must implement ISteamSerializableMessage");

public:
    MsgHdr Header;
    TBody Body;
public:
    explicit Msg(int payloadReserve = 0)
    {
        Payload.reserve(payloadReserve);

        Header.SetEMsg(Body.GetEMsg());
    }

    Msg(const Msg<TBody>& other, int payloadReserve = 0)
        : Msg(payloadReserve)
    {
        Header.TargetJobID = other.Header.SourceJobID;
    }

    explicit Msg(const IPacketMsg& packet)
    {
        PacketMsg* packetMsg = dynamic_cast<PacketMsg*>(&packet);
        if (!packetMsg)
            throw std::runtime_error("Msg<TBody> used for wrong packet type");

        Header = packetMsg->Header;

        const std::vector<byte>& data = packetMsg->GetData();
        size_t offset = packetMsg->GetBodyOffset();

        Stream stream;

        // In a real implementation, Stream would wrap data+offset.
        Body.Deserialize(stream);

        if (offset < data.size())
        {
            Payload.insert(Payload.end(),
                           data.begin() + offset,
                           data.end());
        }
    }

public:
    bool IsProto() const override
    {
        return false;
    }

    EMsg MsgType() const override
    {
        return Header.Msg;
    }

    std::vector<byte> Serialize() override
    {
        Stream stream;

        Header.Serialize(stream);
        Body.Serialize(stream);

        for (byte b : Payload)
            stream.Write(b);

        return std::vector<byte>(); // replace once Stream supports extraction
    }
};
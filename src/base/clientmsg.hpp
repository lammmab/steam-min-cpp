// GPT
#include "base/msgbase.hpp"
#include "base/generated/Stream.hpp"

namespace Steam::Messaging::ClientMessages {
    template <typename TBody>
    class Msg : public Steam::Messaging::Messages::MsgBaseHdr<Steam::Internal::MsgHdr>
    {
        static_assert(std::is_base_of<Steam::Internal::ISteamSerializableMessage, TBody>::value,
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

        explicit Msg(const Steam::Messaging::Packets::IPacketMsg& packet)
        {
            const Steam::Messaging::Packets::PacketMsg* packetMsg = dynamic_cast<const Steam::Messaging::Packets::PacketMsg*>(&packet);
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

        Steam::Internal::Enums::EMsg MsgType() const override
        {
            return Header.Msg;
        }

        std::vector<byte> Serialize() const override 
        {
            Stream stream(StreamingMode::Write);

            Header.Serialize(stream);
            Body.Serialize(stream);
            stream.Write(Payload().data(), Payload().size());

            return stream.MoveBuffer();
        }
    };
}
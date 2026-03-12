// GPT
#pragma once
#include "base/msgbase.hpp"
#include "base/generated/Stream.hpp"
#include "base/packetbase.hpp"

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

    class MsgProtoHdr : public Steam::Messaging::Messages::MsgBaseHdr<Steam::Internal::MsgHdrProtoBuf>
    {
    public:
        explicit MsgProtoHdr(int payloadReserve = 0)
        {
            Payload().reserve(payloadReserve);
        }

        explicit MsgProtoHdr(Steam::Internal::Enums::EMsg eMsg, int payloadReserve = 64)
        {
            Payload().reserve(payloadReserve);
            Header.SetEMsg(eMsg);
        }

        explicit MsgProtoHdr(const Steam::Messaging::Packets::IPacketMsg& packet)
        {
            const auto* packetMsg =
                dynamic_cast<const Steam::Messaging::Packets::PacketClientMsgProtobuf*>(&packet);

            if (!packetMsg)
                throw std::runtime_error("MsgProto used for non-protobuf packet");

            Header = packetMsg->header;
        }

    public:
        bool IsProto() const override
        {
            return true;
        }

        Steam::Internal::Enums::EMsg MsgType() const override
        {
            return Header.Msg;
        }

        std::vector<byte> Serialize() const override
        {
            throw std::runtime_error(
                "MsgProtoHdr is read-only. Use MsgProto<TBody> to serialize.");
        }
    };

    
    template <typename TBody>
    class MsgProto : public MsgProtoHdr
    {

    public:
        TBody Body;

    public:

        explicit MsgProto(Steam::Internal::Enums::EMsg eMsg, int payloadReserve = 0)
            : MsgProtoHdr(payloadReserve)
        {
            Header.SetEMsg(eMsg);
        }

        MsgProto(Steam::Internal::Enums::EMsg eMsg,
                const Steam::Messaging::Messages::MsgBaseHdr<Steam::Internal::MsgHdrProtoBuf>& msg,
                int payloadReserve = 0)
            : MsgProto(eMsg, payloadReserve)
        {
            Header.proto.set_jobid_target(msg.Header.proto.jobid_source());
        }

        explicit MsgProto(const Steam::Messaging::Packets::IPacketMsg& packet)
            : MsgProto(packet.MsgType())
        {
            const auto* packetMsg =
                dynamic_cast<const Steam::Messaging::Packets::PacketClientMsgProtobuf*>(&packet);

            if (!packetMsg)
                throw std::runtime_error("MsgProto<TBody> used for non-protobuf packet");

            Header = packetMsg->header;

            const std::vector<byte>& data = packetMsg->GetData();
            size_t offset = packetMsg->bodyOffset;

            Stream stream(data, offset, StreamingMode::Read);

            Body.ParseFromArray(data.data() + offset, data.size() - offset);

            if (offset < data.size())
            {
                Payload().insert(Payload().end(),
                                data.begin() + offset,
                                data.end());
            }
        }

    public:

        std::vector<byte> Serialize() const override
        {
            Stream stream(StreamingMode::Write);

            Header.Serialize(stream);

            std::string data;
            if (!Body.SerializeToString(&data))
                throw std::runtime_error("Failed to serialize protobuf body");

            stream.Write(data.data(), data.size());
            stream.Write(Payload().data(), Payload().size());

            return stream.MoveBuffer();
        }
    };
}
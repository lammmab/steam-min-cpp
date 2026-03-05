// GPT
#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "base/generated/SteamLanguageInternal.h"
#include "base/generated/Stream.h"

// Packet interface (read-only, header + payload) 
class IPacketMsg { 
    public: virtual ~IPacketMsg() = default; 
    virtual bool IsProto() const = 0; 
    virtual SteamInternal::EMsg MsgType() const = 0; 
    virtual uint64_t TargetJobID() const = 0; 
    virtual uint64_t SourceJobID() const = 0; 
    virtual const std::vector<uint8_t>& GetData() const = 0; 
};

// Protobuf-backed packet
class PacketClientMsgProtobuf : public IPacketMsg {
public:
    SteamInternal::EMsg msgType;
    SteamInternal::Internal::MsgHdrProtoBuf header;
    std::vector<uint8_t> payload;
    size_t bodyOffset = 0;

    PacketClientMsgProtobuf(SteamInternal::EMsg eMsg, const std::vector<uint8_t>& data)
        : msgType(eMsg), payload(data) 
    {
        Stream s(payload,StreamingMode::Read);
        header.Deserialize(s);
        bodyOffset = s.Position();
    }

    bool IsProto() const override { return true; }
    SteamInternal::EMsg MsgType() const override { return msgType; }
    uint64_t TargetJobID() const override { return header.proto.jobid_target(); }
    uint64_t SourceJobID() const override { return header.proto.jobid_source(); }
    const std::vector<uint8_t>& GetData() const override { return payload; }
    const size_t GetBodyOffset() { return bodyOffset; }
};

class PacketClientMsg : public IPacketMsg {
public:
    SteamInternal::EMsg msgType;
    SteamInternal::Internal::ExtendedClientMsgHdr header;
    std::vector<uint8_t> payload;
    size_t bodyOffset = 0;

    PacketClientMsg(SteamInternal::EMsg eMsg, const std::vector<uint8_t>& data)
        : msgType(eMsg), payload(data) 
    {
        Stream s(payload,StreamingMode::Read);
        header.Deserialize(s);
        bodyOffset = s.Position();
    }

    bool IsProto() const override { return false; }
    SteamInternal::EMsg MsgType() const override { return msgType; }
    uint64_t TargetJobID() const override { return header.targetJobID; }
    uint64_t SourceJobID() const override { return header.sourceJobID; }
    const std::vector<uint8_t>& GetData() const override { return payload; }
};

class PacketMsg : public IPacketMsg {
public:
    SteamInternal::EMsg msgType;
    SteamInternal::Internal::MsgHdr header;
    std::vector<uint8_t> payload;
    size_t bodyOffset = 0;

    PacketMsg(SteamInternal::EMsg eMsg, const std::vector<uint8_t>& data)
        : msgType(eMsg), payload(data) 
    {
        Stream s(payload,StreamingMode::Read);
        header.Deserialize(s);
        bodyOffset = s.Position();
    }

    bool IsProto() const override { return false; }
    SteamInternal::EMsg MsgType() const override { return msgType; }
    uint64_t TargetJobID() const override { return header.targetJobID; }
    uint64_t SourceJobID() const override { return header.sourceJobID; }
    const std::vector<uint8_t>& GetData() const override { return payload; }
};
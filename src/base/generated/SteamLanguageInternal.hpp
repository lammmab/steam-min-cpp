// This file was majorly automatically generated.
// Some changes were made because I was too lazy to make the C++ generator extremely robust.
// Avoid modifying unless strictly necessary.
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#include "utils/stream.hpp"
#include "utils/steam_utils.hpp"
#include "protogen/steammessages_base.pb.h"

namespace Steam::Internal
{

	struct SteamID
    {
    private:
        uint64_t value = 0;

        static constexpr int ACCOUNT_ID_SHIFT   = 0;
        static constexpr int INSTANCE_SHIFT     = 32;
        static constexpr int ACCOUNT_TYPE_SHIFT = 52;
        static constexpr int UNIVERSE_SHIFT     = 56;

        static constexpr uint64_t ACCOUNT_ID_MASK   = 0xFFFFFFFFULL;
        static constexpr uint64_t INSTANCE_MASK     = 0xFFFFFULL;
        static constexpr uint64_t ACCOUNT_TYPE_MASK = 0xFULL;
        static constexpr uint64_t UNIVERSE_MASK     = 0xFFULL;

    public:
        SteamID() = default;

        SteamID(
            uint32_t accountID,
            uint32_t instance,
            Steam::Internal::Enums::EUniverse universe,
            Steam::Internal::Enums::EAccountType accountType)
        {
            value =
                ((uint64_t)accountID << ACCOUNT_ID_SHIFT) |
                ((uint64_t)instance << INSTANCE_SHIFT) |
                ((uint64_t)accountType << ACCOUNT_TYPE_SHIFT) |
                ((uint64_t)universe << UNIVERSE_SHIFT);
        }

        SteamID(uint64_t val) {
            value = val;
        }

        uint64_t ConvertToUInt64() const
        {
            return value;
        }
    };

    struct GameID
    {
    private:
        uint64_t value;

    public:
        GameID() : value(0) {}
        explicit GameID(uint64_t v) : value(v) {}

        uint64_t ConvertToUInt64() const
        {
            return value;
        }
    };

	struct ISteamSerializable
	{
		virtual void Serialize(Steam::Utils::Stream& stream) const = 0;
		virtual void Deserialize( Steam::Utils::Stream& stream ) = 0;
		virtual ~ISteamSerializable() = default;
	};

	struct ISteamSerializableHeader : public ISteamSerializable
	{
		virtual void SetEMsg( Steam::Internal::Enums::EMsg msg ) = 0;
	};

	struct ISteamSerializableMessage : public ISteamSerializable
	{
		virtual Steam::Internal::Enums::EMsg GetEMsg() const = 0;
	};

	struct IGCSerializableHeader : public ISteamSerializable
	{
		virtual void SetEMsg( uint32_t msg ) = 0;
	};

	struct IGCSerializableMessage : public ISteamSerializable
	{
		virtual uint32_t GetEMsg() = 0;
		virtual bool IsProto() const = 0;
	};


	struct UdpHeader : public ISteamSerializable
	{
		static const  uint32_t MAGIC = 0x31305356;
		// Static size: 4
		uint32_t magic;
		// Static size: 2
		uint16_t payloadSize;
		// Static size: 1
		Steam::Internal::Enums::EUdpPacketType packetType;
		// Static size: 1
		unsigned char flags;
		// Static size: 4
		uint32_t sourceConnID;
		// Static size: 4
		uint32_t destConnID;
		// Static size: 4
		uint32_t seqThis;
		// Static size: 4
		uint32_t seqAck;
		// Static size: 4
		uint32_t packetsInMsg;
		// Static size: 4
		uint32_t msgStartSeq;
		// Static size: 4
		uint32_t msgSize;

		UdpHeader()
		{
			magic = UdpHeader::MAGIC;
			payloadSize = 0;
			packetType = Steam::Internal::Enums::EUdpPacketType::Invalid;
			flags = 0;
			sourceConnID = 512;
			destConnID = 0;
			seqThis = 0;
			seqAck = 0;
			packetsInMsg = 0;
			msgStartSeq = 0;
			msgSize = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(magic);
			stream.Write(payloadSize);
			stream.Write((byte)packetType);
			stream.Write(flags);
			stream.Write(sourceConnID);
			stream.Write(destConnID);
			stream.Write(seqThis);
			stream.Write(seqAck);
			stream.Write(packetsInMsg);
			stream.Write(msgStartSeq);
			stream.Write(msgSize);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			magic = (uint32_t)stream.Read<int32_t>();
			payloadSize = (uint16_t)stream.Read<int16_t>();
			packetType = (Steam::Internal::Enums::EUdpPacketType)stream.Read<unsigned char>();
			flags = (unsigned char)stream.Read<unsigned char>();
			sourceConnID = (uint32_t)stream.Read<int32_t>();
			destConnID = (uint32_t)stream.Read<int32_t>();
			seqThis = (uint32_t)stream.Read<int32_t>();
			seqAck = (uint32_t)stream.Read<int32_t>();
			packetsInMsg = (uint32_t)stream.Read<int32_t>();
			msgStartSeq = (uint32_t)stream.Read<int32_t>();
			msgSize = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct ChallengeData : public ISteamSerializable
	{
		static const  uint32_t CHALLENGE_MASK = 0xA426DF2B;
		// Static size: 4
		uint32_t challengeValue;
		// Static size: 4
		uint32_t serverLoad;

		ChallengeData()
		{
			challengeValue = 0;
			serverLoad = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(challengeValue);
			stream.Write(serverLoad);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			challengeValue = (uint32_t)stream.Read<int32_t>();
			serverLoad = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct ConnectData : public ISteamSerializable
	{
		static const  uint32_t CHALLENGE_MASK = ChallengeData::CHALLENGE_MASK;
		// Static size: 4
		uint32_t challengeValue;

		ConnectData()
		{
			challengeValue = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(challengeValue);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			challengeValue = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct Accept : public ISteamSerializable
	{

		Accept()
		{
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{


		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
		}
	};

	struct Datagram : public ISteamSerializable
	{

		Datagram()
		{
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{


		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
		}
	};

	struct Disconnect : public ISteamSerializable
	{

		Disconnect()
		{
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{


		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct MsgHdr : public ISteamSerializableHeader
	{
		void SetEMsg(Steam::Internal::Enums::EMsg msg) { Msg = msg; }

		// Static size: 4
		Steam::Internal::Enums::EMsg Msg;
		// Static size: 8
		uint64_t targetJobID;
		// Static size: 8
		uint64_t sourceJobID;

		MsgHdr()
		{
			Msg = Steam::Internal::Enums::EMsg::Invalid;
			targetJobID = UINT64_MAX;
			sourceJobID = UINT64_MAX;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)Msg);
			stream.Write(targetJobID);
			stream.Write(sourceJobID);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			Msg = (Steam::Internal::Enums::EMsg)stream.Read<int32_t>();
			targetJobID = (uint64_t)stream.Read<int64_t>();
			sourceJobID = (uint64_t)stream.Read<int64_t>();
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct ExtendedClientMsgHdr : public ISteamSerializableHeader
	{
		void SetEMsg(Steam::Internal::Enums::EMsg msg) { Msg = msg; }

		// Static size: 4
		Steam::Internal::Enums::EMsg Msg;
		// Static size: 1
		unsigned char headerSize;
		// Static size: 2
		uint16_t headerVersion;
		// Static size: 8
		uint64_t targetJobID;
		// Static size: 8
		uint64_t sourceJobID;
		// Static size: 1
		unsigned char headerCanary;
		// Static size: 8
		private:
			uint64_t steamID;
		public:
			SteamID GetsteamID() const { return SteamID(steamID); }
			void SetsteamID(const SteamID& val) { steamID = val.ConvertToUInt64(); }
		// Static size: 4
		int32_t sessionID;

		ExtendedClientMsgHdr()
		{
			Msg = Steam::Internal::Enums::EMsg::Invalid;
			headerSize = 36;
			headerVersion = 2;
			targetJobID = UINT64_MAX;
			sourceJobID = UINT64_MAX;
			headerCanary = 239;
			steamID = 0;
			sessionID = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)Msg);
			stream.Write(headerSize);
			stream.Write(headerVersion);
			stream.Write(targetJobID);
			stream.Write(sourceJobID);
			stream.Write(headerCanary);
			stream.Write(steamID);
			stream.Write(sessionID);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			Msg = (Steam::Internal::Enums::EMsg)stream.Read<int32_t>();
			headerSize = (unsigned char)stream.Read<unsigned char>();
			headerVersion = (uint16_t)stream.Read<int16_t>();
			targetJobID = (uint64_t)stream.Read<int64_t>();
			sourceJobID = (uint64_t)stream.Read<int64_t>();
			headerCanary = (unsigned char)stream.Read<unsigned char>();
			steamID = (uint64_t)stream.Read<int64_t>();
			sessionID = (int32_t)stream.Read<int32_t>();
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct MsgHdrProtoBuf : public ISteamSerializableHeader
	{
		void SetEMsg(Steam::Internal::Enums::EMsg msg) { Msg = msg; }

		// Static size: 4
		Steam::Internal::Enums::EMsg Msg;
		// Static size: 4
		mutable int32_t headerLength;
		// Static size: 0
		CMsgProtoBufHeader proto;

		MsgHdrProtoBuf()
		{
			Msg = Steam::Internal::Enums::EMsg::Invalid;
			headerLength = 0;
			proto = CMsgProtoBufHeader();
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{
			std::string protoData;
			if (!proto.SerializeToString(&protoData))
			{
				throw std::runtime_error("Failed to serialize proto message");
			}
			headerLength = protoData.size();

			stream.Write((int)Steam::Utils::MsgUtil::make_msg(Msg, true));
			stream.Write(headerLength);
			stream.Write(protoData.data(), protoData.size());
		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			Msg = (Steam::Internal::Enums::EMsg)Steam::Utils::MsgUtil::get_msg(static_cast<uint32_t>(stream.Read<int32_t>()));
			headerLength = (int32_t)stream.Read<int32_t>();
			if (headerLength < 0) throw std::out_of_range("Negative length");
			std::vector<uint8_t> buffer(headerLength);
			stream.Read(buffer.data(), headerLength);

			if (!proto.ParseFromArray(buffer.data(), headerLength))
			{
				throw std::runtime_error("Failed to parse protobuf message");
			}
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct MsgGCHdrProtoBuf : public IGCSerializableHeader
	{
		void SetEMsg(uint32_t msg) { Msg = msg; }

		// Static size: 4
		uint32_t Msg;
		// Static size: 4
		mutable int32_t headerLength;
		// Static size: 0
		CMsgProtoBufHeader proto;

		MsgGCHdrProtoBuf()
		{
			Msg = 0;
			headerLength = 0;
			proto = CMsgProtoBufHeader();
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{
			std::string protoData;
			if (!proto.SerializeToString(&protoData))
			{
				throw std::runtime_error("Failed to serialize proto message");
			}
			headerLength = protoData.size();

			stream.Write(Steam::Utils::MsgUtil::make_gc_msg(Msg, true));
			stream.Write(headerLength);
		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			Msg = (uint32_t)Steam::Utils::MsgUtil::get_gc_msg(static_cast<uint32_t>(stream.Read<int32_t>()));
			headerLength = (int32_t)stream.Read<int32_t>();
			if (headerLength < 0) throw std::out_of_range("Negative length");
			
			std::vector<uint8_t> buffer(headerLength);
			stream.Read(buffer.data(), headerLength);

			if (!proto.ParseFromArray(buffer.data(), headerLength))
			{
				throw std::runtime_error("Failed to parse protobuf message");
			}
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct MsgGCHdr : public IGCSerializableHeader
	{
		void SetEMsg(uint32_t /*msg*/) { }

		// Static size: 2
		uint16_t headerVersion;
		// Static size: 8
		uint64_t targetJobID;
		// Static size: 8
		uint64_t sourceJobID;

		MsgGCHdr()
		{
			headerVersion = 1;
			targetJobID = UINT64_MAX;
			sourceJobID = UINT64_MAX;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(headerVersion);
			stream.Write(targetJobID);
			stream.Write(sourceJobID);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			headerVersion = (uint16_t)stream.Read<int16_t>();
			targetJobID = (uint64_t)stream.Read<int64_t>();
			sourceJobID = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientJustStrings : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::Invalid; }


		MsgClientJustStrings()
		{
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{


		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
		}
	};

	struct MsgClientGenericResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::Invalid; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;

		MsgClientGenericResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
		}
	};

	struct MsgChannelEncryptRequest : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ChannelEncryptRequest; }

		static const  uint32_t PROTOCOL_VERSION = 1;
		// Static size: 4
		uint32_t protocolVersion;
		// Static size: 4
		Steam::Internal::Enums::EUniverse universe;

		MsgChannelEncryptRequest()
		{
			protocolVersion = MsgChannelEncryptRequest::PROTOCOL_VERSION;
			universe = Steam::Internal::Enums::EUniverse::Invalid;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(protocolVersion);
			stream.Write((int)universe);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			protocolVersion = (uint32_t)stream.Read<int32_t>();
			universe = (Steam::Internal::Enums::EUniverse)stream.Read<int32_t>();
		}
	};

	struct MsgChannelEncryptResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ChannelEncryptResponse; }

		// Static size: 4
		uint32_t protocolVersion;
		// Static size: 4
		uint32_t keySize;

		MsgChannelEncryptResponse()
		{
			protocolVersion = MsgChannelEncryptRequest::PROTOCOL_VERSION;
			keySize = 128;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(protocolVersion);
			stream.Write(keySize);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			protocolVersion = (uint32_t)stream.Read<int32_t>();
			keySize = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgChannelEncryptResult : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ChannelEncryptResult; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;

		MsgChannelEncryptResult()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientLogon : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientLogon; }

		static const  uint32_t ObfuscationMask = 0xBAADF00D;
		static const  uint32_t CurrentProtocol = 65581;
		static const  uint32_t ProtocolVerMajorMask = 0xFFFF0000;
		static const  uint32_t ProtocolVerMinorMask = 0xFFFF;
		static const  uint16_t ProtocolVerMinorMinGameServers = 4;
		static const  uint16_t ProtocolVerMinorMinForSupportingEMsgMulti = 12;
		static const  uint16_t ProtocolVerMinorMinForSupportingEMsgClientEncryptPct = 14;
		static const  uint16_t ProtocolVerMinorMinForExtendedMsgHdr = 17;
		static const  uint16_t ProtocolVerMinorMinForCellId = 18;
		static const  uint16_t ProtocolVerMinorMinForSessionIDLast = 19;
		static const  uint16_t ProtocolVerMinorMinForServerAvailablityMsgs = 24;
		static const  uint16_t ProtocolVerMinorMinClients = 25;
		static const  uint16_t ProtocolVerMinorMinForOSType = 26;
		static const  uint16_t ProtocolVerMinorMinForCegApplyPESig = 27;
		static const  uint16_t ProtocolVerMinorMinForMarketingMessages2 = 27;
		static const  uint16_t ProtocolVerMinorMinForAnyProtoBufMessages = 28;
		static const  uint16_t ProtocolVerMinorMinForProtoBufLoggedOffMessage = 28;
		static const  uint16_t ProtocolVerMinorMinForProtoBufMultiMessages = 28;
		static const  uint16_t ProtocolVerMinorMinForSendingProtocolToUFS = 30;
		static const  uint16_t ProtocolVerMinorMinForMachineAuth = 33;
		static const  uint16_t ProtocolVerMinorMinForSessionIDLastAnon = 36;
		static const  uint16_t ProtocolVerMinorMinForEnhancedAppList = 40;
		static const  uint16_t ProtocolVerMinorMinForSteamGuardNotificationUI = 41;
		static const  uint16_t ProtocolVerMinorMinForProtoBufServiceModuleCalls = 42;
		static const  uint16_t ProtocolVerMinorMinForGzipMultiMessages = 43;
		static const  uint16_t ProtocolVerMinorMinForNewVoiceCallAuthorize = 44;
		static const  uint16_t ProtocolVerMinorMinForClientInstanceIDs = 44;

		MsgClientLogon()
		{
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{


		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
		}
	};

	struct MsgClientVACBanStatus : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientVACBanStatus; }

		// Static size: 4
		uint32_t numBans;

		MsgClientVACBanStatus()
		{
			numBans = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(numBans);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			numBans = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientAppUsageEvent : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientAppUsageEvent; }

		// Static size: 4
		Steam::Internal::Enums::EAppUsageEvent appUsageEvent;
		// Static size: 8
		private:
			uint64_t gameID;
		public:
			GameID GetgameID() const { return GameID(gameID); }
			void SetgameID(const GameID& val) { gameID = val.ConvertToUInt64(); }
		// Static size: 2
		uint16_t offline;

		MsgClientAppUsageEvent()
		{
			appUsageEvent = Steam::Internal::Enums::EAppUsageEvent::Unknown;
			gameID = 0;
			offline = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)appUsageEvent);
			stream.Write(gameID);
			stream.Write(offline);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			appUsageEvent = (Steam::Internal::Enums::EAppUsageEvent)stream.Read<int32_t>();
			gameID = (uint64_t)stream.Read<int64_t>();
			offline = (uint16_t)stream.Read<int16_t>();
		}
	};

	struct MsgClientUpdateGuestPassesList : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientUpdateGuestPassesList; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 4
		int32_t countGuestPassesToGive;
		// Static size: 4
		int32_t countGuestPassesToRedeem;

		MsgClientUpdateGuestPassesList()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			countGuestPassesToGive = 0;
			countGuestPassesToRedeem = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(countGuestPassesToGive);
			stream.Write(countGuestPassesToRedeem);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			countGuestPassesToGive = (int32_t)stream.Read<int32_t>();
			countGuestPassesToRedeem = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientP2PIntroducerMessage : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientP2PIntroducerMessage; }

		// Static size: 8
		private:
			uint64_t steamID;
		public:
			SteamID GetsteamID() const { return SteamID(steamID); }
			void SetsteamID(const SteamID& val) { steamID = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EIntroducerRouting routingType;
		// Static size: 1450
		std::vector<unsigned char> data;
		// Static size: 4
		uint32_t dataLen;

		MsgClientP2PIntroducerMessage()
		{
			steamID = 0;
			routingType = Steam::Internal::Enums::EIntroducerRouting::Unknown;
			data = std::vector<unsigned char>(1450);
			dataLen = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamID);
			stream.Write((int)routingType);
			stream.Write(data);
			stream.Write(dataLen);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamID = (uint64_t)stream.Read<int64_t>();
			routingType = (Steam::Internal::Enums::EIntroducerRouting)stream.Read<int32_t>();
			data = stream.ReadArray<unsigned char>(1450);
			dataLen = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSBeginSession : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientOGSBeginSession; }

		// Static size: 1
		unsigned char accountType;
		// Static size: 8
		private:
			uint64_t accountId;
		public:
			SteamID GetaccountId() const { return SteamID(accountId); }
			void SetaccountId(const SteamID& val) { accountId = val.ConvertToUInt64(); }
		// Static size: 4
		uint32_t appId;
		// Static size: 4
		uint32_t timeStarted;

		MsgClientOGSBeginSession()
		{
			accountType = 0;
			accountId = 0;
			appId = 0;
			timeStarted = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(accountType);
			stream.Write(accountId);
			stream.Write(appId);
			stream.Write(timeStarted);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			accountType = (unsigned char)stream.Read<unsigned char>();
			accountId = (uint64_t)stream.Read<int64_t>();
			appId = (uint32_t)stream.Read<int32_t>();
			timeStarted = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSBeginSessionResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientOGSBeginSessionResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 1
		unsigned char collectingAny;
		// Static size: 1
		unsigned char collectingDetails;
		// Static size: 8
		uint64_t sessionId;

		MsgClientOGSBeginSessionResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			collectingAny = 0;
			collectingDetails = 0;
			sessionId = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(collectingAny);
			stream.Write(collectingDetails);
			stream.Write(sessionId);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			collectingAny = (unsigned char)stream.Read<unsigned char>();
			collectingDetails = (unsigned char)stream.Read<unsigned char>();
			sessionId = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientOGSEndSession : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientOGSEndSession; }

		// Static size: 8
		uint64_t sessionId;
		// Static size: 4
		uint32_t timeEnded;
		// Static size: 4
		int32_t reasonCode;
		// Static size: 4
		int32_t countAttributes;

		MsgClientOGSEndSession()
		{
			sessionId = 0;
			timeEnded = 0;
			reasonCode = 0;
			countAttributes = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(sessionId);
			stream.Write(timeEnded);
			stream.Write(reasonCode);
			stream.Write(countAttributes);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			sessionId = (uint64_t)stream.Read<int64_t>();
			timeEnded = (uint32_t)stream.Read<int32_t>();
			reasonCode = (int32_t)stream.Read<int32_t>();
			countAttributes = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSEndSessionResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientOGSEndSessionResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;

		MsgClientOGSEndSessionResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSWriteRow : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientOGSWriteRow; }

		// Static size: 8
		uint64_t sessionId;
		// Static size: 4
		int32_t countAttributes;

		MsgClientOGSWriteRow()
		{
			sessionId = 0;
			countAttributes = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(sessionId);
			stream.Write(countAttributes);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			sessionId = (uint64_t)stream.Read<int64_t>();
			countAttributes = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientGetFriendsWhoPlayGame : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientGetFriendsWhoPlayGame; }

		// Static size: 8
		private:
			uint64_t gameId;
		public:
			GameID GetgameId() const { return GameID(gameId); }
			void SetgameId(const GameID& val) { gameId = val.ConvertToUInt64(); }

		MsgClientGetFriendsWhoPlayGame()
		{
			gameId = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(gameId);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			gameId = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientGetFriendsWhoPlayGameResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientGetFriendsWhoPlayGameResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 8
		private:
			uint64_t gameId;
		public:
			GameID GetgameId() const { return GameID(gameId); }
			void SetgameId(const GameID& val) { gameId = val.ConvertToUInt64(); }
		// Static size: 4
		uint32_t countFriends;

		MsgClientGetFriendsWhoPlayGameResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			gameId = 0;
			countFriends = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(gameId);
			stream.Write(countFriends);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			gameId = (uint64_t)stream.Read<int64_t>();
			countFriends = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSPerformHardwareSurvey : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSPerformHardwareSurvey; }

		// Static size: 4
		uint32_t flags;

		MsgGSPerformHardwareSurvey()
		{
			flags = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(flags);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			flags = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSGetPlayStatsResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSGetPlayStatsResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 4
		int32_t rank;
		// Static size: 4
		uint32_t lifetimeConnects;
		// Static size: 4
		uint32_t lifetimeMinutesPlayed;

		MsgGSGetPlayStatsResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			rank = 0;
			lifetimeConnects = 0;
			lifetimeMinutesPlayed = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(rank);
			stream.Write(lifetimeConnects);
			stream.Write(lifetimeMinutesPlayed);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			rank = (int32_t)stream.Read<int32_t>();
			lifetimeConnects = (uint32_t)stream.Read<int32_t>();
			lifetimeMinutesPlayed = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSGetReputationResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSGetReputationResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 4
		uint32_t reputationScore;
		// Static size: 1
		unsigned char banned;
		// Static size: 4
		uint32_t bannedIp;
		// Static size: 2
		uint16_t bannedPort;
		// Static size: 8
		uint64_t bannedGameId;
		// Static size: 4
		uint32_t timeBanExpires;

		MsgGSGetReputationResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			reputationScore = 0;
			banned = 0;
			bannedIp = 0;
			bannedPort = 0;
			bannedGameId = 0;
			timeBanExpires = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(reputationScore);
			stream.Write(banned);
			stream.Write(bannedIp);
			stream.Write(bannedPort);
			stream.Write(bannedGameId);
			stream.Write(timeBanExpires);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			reputationScore = (uint32_t)stream.Read<int32_t>();
			banned = (unsigned char)stream.Read<unsigned char>();
			bannedIp = (uint32_t)stream.Read<int32_t>();
			bannedPort = (uint16_t)stream.Read<int16_t>();
			bannedGameId = (uint64_t)stream.Read<int64_t>();
			timeBanExpires = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSDeny : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSDeny; }

		// Static size: 8
		private:
			uint64_t steamId;
		public:
			SteamID GetsteamId() const { return SteamID(steamId); }
			void SetsteamId(const SteamID& val) { steamId = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EDenyReason denyReason;

		MsgGSDeny()
		{
			steamId = 0;
			denyReason = Steam::Internal::Enums::EDenyReason::Unknown;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamId);
			stream.Write((int)denyReason);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamId = (uint64_t)stream.Read<int64_t>();
			denyReason = (Steam::Internal::Enums::EDenyReason)stream.Read<int32_t>();
		}
	};

	struct MsgGSApprove : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSApprove; }

		// Static size: 8
		private:
			uint64_t steamId;
		public:
			SteamID GetsteamId() const { return SteamID(steamId); }
			void SetsteamId(const SteamID& val) { steamId = val.ConvertToUInt64(); }

		MsgGSApprove()
		{
			steamId = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamId);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamId = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgGSKick : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSKick; }

		// Static size: 8
		private:
			uint64_t steamId;
		public:
			SteamID GetsteamId() const { return SteamID(steamId); }
			void SetsteamId(const SteamID& val) { steamId = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EDenyReason denyReason;
		// Static size: 4
		int32_t waitTilMapChange;

		MsgGSKick()
		{
			steamId = 0;
			denyReason = Steam::Internal::Enums::EDenyReason::Unknown;
			waitTilMapChange = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamId);
			stream.Write((int)denyReason);
			stream.Write(waitTilMapChange);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamId = (uint64_t)stream.Read<int64_t>();
			denyReason = (Steam::Internal::Enums::EDenyReason)stream.Read<int32_t>();
			waitTilMapChange = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSGetUserGroupStatus : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSGetUserGroupStatus; }

		// Static size: 8
		private:
			uint64_t steamIdUser;
		public:
			SteamID GetsteamIdUser() const { return SteamID(steamIdUser); }
			void SetsteamIdUser(const SteamID& val) { steamIdUser = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdGroup;
		public:
			SteamID GetsteamIdGroup() const { return SteamID(steamIdGroup); }
			void SetsteamIdGroup(const SteamID& val) { steamIdGroup = val.ConvertToUInt64(); }

		MsgGSGetUserGroupStatus()
		{
			steamIdUser = 0;
			steamIdGroup = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdUser);
			stream.Write(steamIdGroup);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdUser = (uint64_t)stream.Read<int64_t>();
			steamIdGroup = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgGSGetUserGroupStatusResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::GSGetUserGroupStatusResponse; }

		// Static size: 8
		private:
			uint64_t steamIdUser;
		public:
			SteamID GetsteamIdUser() const { return SteamID(steamIdUser); }
			void SetsteamIdUser(const SteamID& val) { steamIdUser = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdGroup;
		public:
			SteamID GetsteamIdGroup() const { return SteamID(steamIdGroup); }
			void SetsteamIdGroup(const SteamID& val) { steamIdGroup = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EClanRelationship clanRelationship;
		// Static size: 4
		Steam::Internal::Enums::EClanRank clanRank;

		MsgGSGetUserGroupStatusResponse()
		{
			steamIdUser = 0;
			steamIdGroup = 0;
			clanRelationship = Steam::Internal::Enums::EClanRelationship::None;
			clanRank = Steam::Internal::Enums::EClanRank::None;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdUser);
			stream.Write(steamIdGroup);
			stream.Write((int)clanRelationship);
			stream.Write((int)clanRank);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdUser = (uint64_t)stream.Read<int64_t>();
			steamIdGroup = (uint64_t)stream.Read<int64_t>();
			clanRelationship = (Steam::Internal::Enums::EClanRelationship)stream.Read<int32_t>();
			clanRank = (Steam::Internal::Enums::EClanRank)stream.Read<int32_t>();
		}
	};

	struct MsgClientJoinChat : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientJoinChat; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 1
		unsigned char isVoiceSpeaker;

		MsgClientJoinChat()
		{
			steamIdChat = 0;
			isVoiceSpeaker = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChat);
			stream.Write(isVoiceSpeaker);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			isVoiceSpeaker = (unsigned char)stream.Read<unsigned char>();
		}
	};

	struct MsgClientChatEnter : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientChatEnter; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdFriend;
		public:
			SteamID GetsteamIdFriend() const { return SteamID(steamIdFriend); }
			void SetsteamIdFriend(const SteamID& val) { steamIdFriend = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatRoomType chatRoomType;
		// Static size: 8
		private:
			uint64_t steamIdOwner;
		public:
			SteamID GetsteamIdOwner() const { return SteamID(steamIdOwner); }
			void SetsteamIdOwner(const SteamID& val) { steamIdOwner = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdClan;
		public:
			SteamID GetsteamIdClan() const { return SteamID(steamIdClan); }
			void SetsteamIdClan(const SteamID& val) { steamIdClan = val.ConvertToUInt64(); }
		// Static size: 1
		unsigned char chatFlags;
		// Static size: 4
		Steam::Internal::Enums::EChatRoomEnterResponse enterResponse;
		// Static size: 4
		int32_t numMembers;

		MsgClientChatEnter()
		{
			steamIdChat = 0;
			steamIdFriend = 0;
			chatRoomType = Steam::Internal::Enums::EChatRoomType::Unknown;
			steamIdOwner = 0;
			steamIdClan = 0;
			chatFlags = 0;
			enterResponse = Steam::Internal::Enums::EChatRoomEnterResponse::Unknown;
			numMembers = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChat);
			stream.Write(steamIdFriend);
			stream.Write((int)chatRoomType);
			stream.Write(steamIdOwner);
			stream.Write(steamIdClan);
			stream.Write(chatFlags);
			stream.Write((int)enterResponse);
			stream.Write(numMembers);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			steamIdFriend = (uint64_t)stream.Read<int64_t>();
			chatRoomType = (Steam::Internal::Enums::EChatRoomType)stream.Read<int32_t>();
			steamIdOwner = (uint64_t)stream.Read<int64_t>();
			steamIdClan = (uint64_t)stream.Read<int64_t>();
			chatFlags = (unsigned char)stream.Read<unsigned char>();
			enterResponse = (Steam::Internal::Enums::EChatRoomEnterResponse)stream.Read<int32_t>();
			numMembers = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatMsg : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientChatMsg; }

		// Static size: 8
		private:
			uint64_t steamIdChatter;
		public:
			SteamID GetsteamIdChatter() const { return SteamID(steamIdChatter); }
			void SetsteamIdChatter(const SteamID& val) { steamIdChatter = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdChatRoom;
		public:
			SteamID GetsteamIdChatRoom() const { return SteamID(steamIdChatRoom); }
			void SetsteamIdChatRoom(const SteamID& val) { steamIdChatRoom = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatEntryType chatMsgType;

		MsgClientChatMsg()
		{
			steamIdChatter = 0;
			steamIdChatRoom = 0;
			chatMsgType = Steam::Internal::Enums::EChatEntryType::Invalid;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChatter);
			stream.Write(steamIdChatRoom);
			stream.Write((int)chatMsgType);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChatter = (uint64_t)stream.Read<int64_t>();
			steamIdChatRoom = (uint64_t)stream.Read<int64_t>();
			chatMsgType = (Steam::Internal::Enums::EChatEntryType)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatMemberInfo : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientChatMemberInfo; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatInfoType type;

		MsgClientChatMemberInfo()
		{
			steamIdChat = 0;
			type = Steam::Internal::Enums::EChatInfoType::Unknown;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChat);
			stream.Write((int)type);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			type = (Steam::Internal::Enums::EChatInfoType)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatAction : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientChatAction; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdUserToActOn;
		public:
			SteamID GetsteamIdUserToActOn() const { return SteamID(steamIdUserToActOn); }
			void SetsteamIdUserToActOn(const SteamID& val) { steamIdUserToActOn = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatAction chatAction;

		MsgClientChatAction()
		{
			steamIdChat = 0;
			steamIdUserToActOn = 0;
			chatAction = Steam::Internal::Enums::EChatAction::Unknown;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChat);
			stream.Write(steamIdUserToActOn);
			stream.Write((int)chatAction);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			steamIdUserToActOn = (uint64_t)stream.Read<int64_t>();
			chatAction = (Steam::Internal::Enums::EChatAction)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatActionResult : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientChatActionResult; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdUserActedOn;
		public:
			SteamID GetsteamIdUserActedOn() const { return SteamID(steamIdUserActedOn); }
			void SetsteamIdUserActedOn(const SteamID& val) { steamIdUserActedOn = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatAction chatAction;
		// Static size: 4
		Steam::Internal::Enums::EChatActionResult actionResult;

		MsgClientChatActionResult()
		{
			steamIdChat = 0;
			steamIdUserActedOn = 0;
			chatAction = Steam::Internal::Enums::EChatAction::Unknown;
			actionResult = Steam::Internal::Enums::EChatActionResult::Unknown;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChat);
			stream.Write(steamIdUserActedOn);
			stream.Write((int)chatAction);
			stream.Write((int)actionResult);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			steamIdUserActedOn = (uint64_t)stream.Read<int64_t>();
			chatAction = (Steam::Internal::Enums::EChatAction)stream.Read<int32_t>();
			actionResult = (Steam::Internal::Enums::EChatActionResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatRoomInfo : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientChatRoomInfo; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatInfoType type;

		MsgClientChatRoomInfo()
		{
			steamIdChat = 0;
			type = Steam::Internal::Enums::EChatInfoType::Unknown;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(steamIdChat);
			stream.Write((int)type);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			type = (Steam::Internal::Enums::EChatInfoType)stream.Read<int32_t>();
		}
	};

	struct MsgClientSetIgnoreFriend : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientSetIgnoreFriend; }

		// Static size: 8
		private:
			uint64_t mySteamId;
		public:
			SteamID GetmySteamId() const { return SteamID(mySteamId); }
			void SetmySteamId(const SteamID& val) { mySteamId = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdFriend;
		public:
			SteamID GetsteamIdFriend() const { return SteamID(steamIdFriend); }
			void SetsteamIdFriend(const SteamID& val) { steamIdFriend = val.ConvertToUInt64(); }
		// Static size: 1
		unsigned char ignore;

		MsgClientSetIgnoreFriend()
		{
			mySteamId = 0;
			steamIdFriend = 0;
			ignore = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(mySteamId);
			stream.Write(steamIdFriend);
			stream.Write(ignore);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			mySteamId = (uint64_t)stream.Read<int64_t>();
			steamIdFriend = (uint64_t)stream.Read<int64_t>();
			ignore = (unsigned char)stream.Read<unsigned char>();
		}
	};

	struct MsgClientSetIgnoreFriendResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientSetIgnoreFriendResponse; }

		// Static size: 8
		private:
			uint64_t friendId;
		public:
			SteamID GetfriendId() const { return SteamID(friendId); }
			void SetfriendId(const SteamID& val) { friendId = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EResult result;

		MsgClientSetIgnoreFriendResponse()
		{
			friendId = 0;
			result = Steam::Internal::Enums::EResult::Invalid;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(friendId);
			stream.Write((int)result);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			friendId = (uint64_t)stream.Read<int64_t>();
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientLoggedOff : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientLoggedOff; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 4
		int32_t secMinReconnectHint;
		// Static size: 4
		int32_t secMaxReconnectHint;

		MsgClientLoggedOff()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			secMinReconnectHint = 0;
			secMaxReconnectHint = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(secMinReconnectHint);
			stream.Write(secMaxReconnectHint);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			secMinReconnectHint = (int32_t)stream.Read<int32_t>();
			secMaxReconnectHint = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientLogOnResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientLogOnResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 4
		int32_t outOfGameHeartbeatRateSec;
		// Static size: 4
		int32_t inGameHeartbeatRateSec;
		// Static size: 8
		private:
			uint64_t clientSuppliedSteamId;
		public:
			SteamID GetclientSuppliedSteamId() const { return SteamID(clientSuppliedSteamId); }
			void SetclientSuppliedSteamId(const SteamID& val) { clientSuppliedSteamId = val.ConvertToUInt64(); }
		// Static size: 4
		uint32_t ipPublic;
		// Static size: 4
		uint32_t serverRealTime;

		MsgClientLogOnResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			outOfGameHeartbeatRateSec = 0;
			inGameHeartbeatRateSec = 0;
			clientSuppliedSteamId = 0;
			ipPublic = 0;
			serverRealTime = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(outOfGameHeartbeatRateSec);
			stream.Write(inGameHeartbeatRateSec);
			stream.Write(clientSuppliedSteamId);
			stream.Write(ipPublic);
			stream.Write(serverRealTime);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			outOfGameHeartbeatRateSec = (int32_t)stream.Read<int32_t>();
			inGameHeartbeatRateSec = (int32_t)stream.Read<int32_t>();
			clientSuppliedSteamId = (uint64_t)stream.Read<int64_t>();
			ipPublic = (uint32_t)stream.Read<int32_t>();
			serverRealTime = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientServerUnavailable : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientServerUnavailable; }

		// Static size: 8
		uint64_t jobidSent;
		// Static size: 4
		uint32_t eMsgSent;
		// Static size: 4
		Steam::Internal::Enums::EServerType eServerTypeUnavailable;

		MsgClientServerUnavailable()
		{
			jobidSent = 0;
			eMsgSent = 0;
			eServerTypeUnavailable = Steam::Internal::Enums::EServerType::First;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(jobidSent);
			stream.Write(eMsgSent);
			stream.Write((int)eServerTypeUnavailable);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			jobidSent = (uint64_t)stream.Read<int64_t>();
			eMsgSent = (uint32_t)stream.Read<int32_t>();
			eServerTypeUnavailable = (Steam::Internal::Enums::EServerType)stream.Read<int32_t>();
		}
	};

	struct MsgClientCreateChat : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientCreateChat; }

		// Static size: 4
		Steam::Internal::Enums::EChatRoomType chatRoomType;
		// Static size: 8
		private:
			uint64_t gameId;
		public:
			GameID GetgameId() const { return GameID(gameId); }
			void SetgameId(const GameID& val) { gameId = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdClan;
		public:
			SteamID GetsteamIdClan() const { return SteamID(steamIdClan); }
			void SetsteamIdClan(const SteamID& val) { steamIdClan = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatPermission permissionOfficer;
		// Static size: 4
		Steam::Internal::Enums::EChatPermission permissionMember;
		// Static size: 4
		Steam::Internal::Enums::EChatPermission permissionAll;
		// Static size: 4
		uint32_t membersMax;
		// Static size: 1
		unsigned char chatFlags;
		// Static size: 8
		private:
			uint64_t steamIdFriendChat;
		public:
			SteamID GetsteamIdFriendChat() const { return SteamID(steamIdFriendChat); }
			void SetsteamIdFriendChat(const SteamID& val) { steamIdFriendChat = val.ConvertToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdInvited;
		public:
			SteamID GetsteamIdInvited() const { return SteamID(steamIdInvited); }
			void SetsteamIdInvited(const SteamID& val) { steamIdInvited = val.ConvertToUInt64(); }

		MsgClientCreateChat()
		{
			chatRoomType = Steam::Internal::Enums::EChatRoomType::Unknown;
			gameId = 0;
			steamIdClan = 0;
			// These were 0; this may be completely incorrect now.
			permissionOfficer = Steam::Internal::Enums::EChatPermission::OfficerDefault;
			permissionMember = Steam::Internal::Enums::EChatPermission::MemberDefault;
			permissionAll = Steam::Internal::Enums::EChatPermission::OwnerDefault;
			// End block
			membersMax = 0;
			chatFlags = 0;
			steamIdFriendChat = 0;
			steamIdInvited = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)chatRoomType);
			stream.Write(gameId);
			stream.Write(steamIdClan);
			stream.Write((int)permissionOfficer);
			stream.Write((int)permissionMember);
			stream.Write((int)permissionAll);
			stream.Write(membersMax);
			stream.Write(chatFlags);
			stream.Write(steamIdFriendChat);
			stream.Write(steamIdInvited);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			chatRoomType = (Steam::Internal::Enums::EChatRoomType)stream.Read<int32_t>();
			gameId = (uint64_t)stream.Read<int64_t>();
			steamIdClan = (uint64_t)stream.Read<int64_t>();
			permissionOfficer = (Steam::Internal::Enums::EChatPermission)stream.Read<int32_t>();
			permissionMember = (Steam::Internal::Enums::EChatPermission)stream.Read<int32_t>();
			permissionAll = (Steam::Internal::Enums::EChatPermission)stream.Read<int32_t>();
			membersMax = (uint32_t)stream.Read<int32_t>();
			chatFlags = (unsigned char)stream.Read<unsigned char>();
			steamIdFriendChat = (uint64_t)stream.Read<int64_t>();
			steamIdInvited = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientCreateChatResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientCreateChatResponse; }

		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 4
		Steam::Internal::Enums::EChatRoomType chatRoomType;
		// Static size: 8
		private:
			uint64_t steamIdFriendChat;
		public:
			SteamID GetsteamIdFriendChat() const { return SteamID(steamIdFriendChat); }
			void SetsteamIdFriendChat(const SteamID& val) { steamIdFriendChat = val.ConvertToUInt64(); }

		MsgClientCreateChatResponse()
		{
			result = Steam::Internal::Enums::EResult::Invalid;
			steamIdChat = 0;
			chatRoomType = Steam::Internal::Enums::EChatRoomType::Unknown;
			steamIdFriendChat = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write((int)result);
			stream.Write(steamIdChat);
			stream.Write((int)chatRoomType);
			stream.Write(steamIdFriendChat);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			chatRoomType = (Steam::Internal::Enums::EChatRoomType)stream.Read<int32_t>();
			steamIdFriendChat = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientMarketingMessageUpdate2 : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientMarketingMessageUpdate2; }

		// Static size: 4
		uint32_t marketingMessageUpdateTime;
		// Static size: 4
		uint32_t count;

		MsgClientMarketingMessageUpdate2()
		{
			marketingMessageUpdateTime = 0;
			count = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(marketingMessageUpdateTime);
			stream.Write(count);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			marketingMessageUpdateTime = (uint32_t)stream.Read<int32_t>();
			count = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientGetLegacyGameKey : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientGetLegacyGameKey; }

		// Static size: 4
		uint32_t appId;

		MsgClientGetLegacyGameKey()
		{
			appId = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(appId);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			appId = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientGetLegacyGameKeyResponse : public ISteamSerializableMessage
	{
		Steam::Internal::Enums::EMsg GetEMsg() const { return Steam::Internal::Enums::EMsg::ClientGetLegacyGameKeyResponse; }

		// Static size: 4
		uint32_t appId;
		// Static size: 4
		Steam::Internal::Enums::EResult result;
		// Static size: 4
		uint32_t length;

		MsgClientGetLegacyGameKeyResponse()
		{
			appId = 0;
			result = Steam::Internal::Enums::EResult::Invalid;
			length = 0;
		}

		void Serialize(Steam::Utils::Stream& stream) const
		{

			stream.Write(appId);
			stream.Write((int)result);
			stream.Write(length);

		}

		void Deserialize( Steam::Utils::Stream& stream )
		{
			appId = (uint32_t)stream.Read<int32_t>();
			result = (Steam::Internal::Enums::EResult)stream.Read<int32_t>();
			length = (uint32_t)stream.Read<int32_t>();
		}
	};

} // namespace Steam::Internal

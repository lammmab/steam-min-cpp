// This file was majorly automatically generated.
// Some changes were made because I was too lazy to make the C++ generator extremely robust.
// Avoid modifying unless strictly necessary.
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#include "base/generated/Stream.h"
#include "base/generated/SteamUtils.h"
#include "protogen/steammessages_base.pb.h"

namespace SteamInternal::Internal
{
	struct ISteamSerializable
	{
		virtual void Serialize(Stream& stream) = 0;
		virtual void Deserialize( Stream& stream ) = 0;
		virtual ~ISteamSerializable() = default;
	};

	struct ISteamSerializableHeader : public ISteamSerializable
	{
		virtual void SetEMsg( SteamInternal::EMsg msg ) = 0;
	};

	struct ISteamSerializableMessage : public ISteamSerializable
	{
		virtual SteamInternal::EMsg GetEMsg() const = 0;
	};

	struct IGCSerializableHeader : public ISteamSerializable
	{
		virtual void SetEMsg( uint32_t msg ) = 0;
	};

	struct IGCSerializableMessage : public ISteamSerializable
	{
		virtual uint32_t GetEMsg() = 0;
	};


	struct UdpHeader : public ISteamSerializable
	{
		static const  uint32_t MAGIC = 0x31305356;
		// Static size: 4
		uint32_t magic;
		// Static size: 2
		uint16_t payloadSize;
		// Static size: 1
		EUdpPacketType packetType;
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
			packetType = EUdpPacketType::Invalid;
			flags = 0;
			sourceConnID = 512;
			destConnID = 0;
			seqThis = 0;
			seqAck = 0;
			packetsInMsg = 0;
			msgStartSeq = 0;
			msgSize = 0;
		}

		void Serialize(Stream& stream)
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

		void Deserialize( Stream& stream )
		{
			magic = (uint32_t)stream.Read<int32_t>();
			payloadSize = (uint16_t)stream.Read<int16_t>();
			packetType = (EUdpPacketType)stream.Read<unsigned char>();
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

		void Serialize(Stream& stream)
		{

			stream.Write(challengeValue);
			stream.Write(serverLoad);

		}

		void Deserialize( Stream& stream )
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

		void Serialize(Stream& stream)
		{

			stream.Write(challengeValue);

		}

		void Deserialize( Stream& stream )
		{
			challengeValue = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct Accept : public ISteamSerializable
	{

		Accept()
		{
		}

		void Serialize(Stream& stream)
		{


		}

		void Deserialize( Stream& stream )
		{
		}
	};

	struct Datagram : public ISteamSerializable
	{

		Datagram()
		{
		}

		void Serialize(Stream& stream)
		{


		}

		void Deserialize( Stream& stream )
		{
		}
	};

	struct Disconnect : public ISteamSerializable
	{

		Disconnect()
		{
		}

		void Serialize(Stream& stream)
		{


		}

		void Deserialize( Stream& stream )
		{
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct MsgHdr : public ISteamSerializableHeader
	{
		void SetEMsg(EMsg msg) { Msg = msg; }

		// Static size: 4
		SteamInternal::EMsg Msg;
		// Static size: 8
		uint64_t targetJobID;
		// Static size: 8
		uint64_t sourceJobID;

		MsgHdr()
		{
			Msg = SteamInternal::EMsg::Invalid;
			targetJobID = UINT64_MAX;
			sourceJobID = UINT64_MAX;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)Msg);
			stream.Write(targetJobID);
			stream.Write(sourceJobID);

		}

		void Deserialize( Stream& stream )
		{
			Msg = (SteamInternal::EMsg)stream.Read<int32_t>();
			targetJobID = (uint64_t)stream.Read<int64_t>();
			sourceJobID = (uint64_t)stream.Read<int64_t>();
		}
	};

	// Sequential layout struct (originally StructLayout.Sequential)
	struct ExtendedClientMsgHdr : public ISteamSerializableHeader
	{
		void SetEMsg(SteamInternal::EMsg msg) { Msg = msg; }

		// Static size: 4
		SteamInternal::EMsg Msg;
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
			Msg = SteamInternal::EMsg::Invalid;
			headerSize = 36;
			headerVersion = 2;
			targetJobID = UINT64_MAX;
			sourceJobID = UINT64_MAX;
			headerCanary = 239;
			steamID = 0;
			sessionID = 0;
		}

		void Serialize(Stream& stream)
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

		void Deserialize( Stream& stream )
		{
			Msg = (SteamInternal::EMsg)stream.Read<int32_t>();
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
		void SetEMsg(SteamInternal::EMsg msg) { Msg = msg; }

		// Static size: 4
		SteamInternal::EMsg Msg;
		// Static size: 4
		int32_t headerLength;
		// Static size: 0
		CMsgProtoBufHeader proto;

		MsgHdrProtoBuf()
		{
			Msg = SteamInternal::EMsg::Invalid;
			headerLength = 0;
			proto = CMsgProtoBufHeader();
		}

		void Serialize(Stream& stream)
		{
			std::string protoData;
			if (!proto.SerializeToString(&protoData))
			{
				throw std::runtime_error("Failed to serialize proto message");
			}
			headerLength = static_cast<int>(protoData.size());

			stream.Write((int)MsgUtil::MakeMsg(Msg, true));
			stream.Write(headerLength);
		}

		void Deserialize( Stream& stream )
		{
			Msg = (SteamInternal::EMsg)MsgUtil::GetMsg(static_cast<uint32_t>(stream.Read<int32_t>()));
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
		int32_t headerLength;
		// Static size: 0
		CMsgProtoBufHeader proto;

		MsgGCHdrProtoBuf()
		{
			Msg = 0;
			headerLength = 0;
			proto = CMsgProtoBufHeader();
		}

		void Serialize(Stream& stream)
		{
			std::string protoData;
			if (!proto.SerializeToString(&protoData))
			{
				throw std::runtime_error("Failed to serialize proto message");
			}
			headerLength = static_cast<int>(protoData.size());

			stream.Write(MsgUtil::MakeGCMsg(Msg, true));
			stream.Write(headerLength);

		}

		void Deserialize( Stream& stream )
		{
			Msg = (uint32_t)MsgUtil::GetGCMsg(static_cast<uint32_t>(stream.Read<int32_t>()));
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

		void Serialize(Stream& stream)
		{

			stream.Write(headerVersion);
			stream.Write(targetJobID);
			stream.Write(sourceJobID);

		}

		void Deserialize( Stream& stream )
		{
			headerVersion = (uint16_t)stream.Read<int16_t>();
			targetJobID = (uint64_t)stream.Read<int64_t>();
			sourceJobID = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientJustStrings : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::Invalid; }


		MsgClientJustStrings()
		{
		}

		void Serialize(Stream& stream)
		{


		}

		void Deserialize( Stream& stream )
		{
		}
	};

	struct MsgClientGenericResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::Invalid; }

		// Static size: 4
		EResult result;

		MsgClientGenericResponse()
		{
			result = EResult::Invalid;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
		}
	};

	struct MsgChannelEncryptRequest : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ChannelEncryptRequest; }

		static const  uint32_t PROTOCOL_VERSION = 1;
		// Static size: 4
		uint32_t protocolVersion;
		// Static size: 4
		EUniverse universe;

		MsgChannelEncryptRequest()
		{
			protocolVersion = MsgChannelEncryptRequest::PROTOCOL_VERSION;
			universe = EUniverse::Invalid;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(protocolVersion);
			stream.Write((int)universe);

		}

		void Deserialize( Stream& stream )
		{
			protocolVersion = (uint32_t)stream.Read<int32_t>();
			universe = (EUniverse)stream.Read<int32_t>();
		}
	};

	struct MsgChannelEncryptResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ChannelEncryptResponse; }

		// Static size: 4
		uint32_t protocolVersion;
		// Static size: 4
		uint32_t keySize;

		MsgChannelEncryptResponse()
		{
			protocolVersion = MsgChannelEncryptRequest::PROTOCOL_VERSION;
			keySize = 128;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(protocolVersion);
			stream.Write(keySize);

		}

		void Deserialize( Stream& stream )
		{
			protocolVersion = (uint32_t)stream.Read<int32_t>();
			keySize = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgChannelEncryptResult : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ChannelEncryptResult; }

		// Static size: 4
		EResult result;

		MsgChannelEncryptResult()
		{
			result = EResult::Invalid;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientLogon : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientLogon; }

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

		void Serialize(Stream& stream)
		{


		}

		void Deserialize( Stream& stream )
		{
		}
	};

	struct MsgClientVACBanStatus : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientVACBanStatus; }

		// Static size: 4
		uint32_t numBans;

		MsgClientVACBanStatus()
		{
			numBans = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(numBans);

		}

		void Deserialize( Stream& stream )
		{
			numBans = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientAppUsageEvent : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientAppUsageEvent; }

		// Static size: 4
		EAppUsageEvent appUsageEvent;
		// Static size: 8
		private:
			uint64_t gameID;
		public:
			GameID GetgameID() const { return GameID(gameID); }
			void SetgameID(const GameID& val) { gameID = val.ToUInt64(); }
		// Static size: 2
		uint16_t offline;

		MsgClientAppUsageEvent()
		{
			appUsageEvent = EAppUsageEvent::Unknown;
			gameID = 0;
			offline = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)appUsageEvent);
			stream.Write(gameID);
			stream.Write(offline);

		}

		void Deserialize( Stream& stream )
		{
			appUsageEvent = (EAppUsageEvent)stream.Read<int32_t>();
			gameID = (uint64_t)stream.Read<int64_t>();
			offline = (uint16_t)stream.Read<int16_t>();
		}
	};

	struct MsgClientUpdateGuestPassesList : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientUpdateGuestPassesList; }

		// Static size: 4
		EResult result;
		// Static size: 4
		int32_t countGuestPassesToGive;
		// Static size: 4
		int32_t countGuestPassesToRedeem;

		MsgClientUpdateGuestPassesList()
		{
			result = EResult::Invalid;
			countGuestPassesToGive = 0;
			countGuestPassesToRedeem = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(countGuestPassesToGive);
			stream.Write(countGuestPassesToRedeem);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			countGuestPassesToGive = (int32_t)stream.Read<int32_t>();
			countGuestPassesToRedeem = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientP2PIntroducerMessage : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientP2PIntroducerMessage; }

		// Static size: 8
		private:
			uint64_t steamID;
		public:
			SteamID GetsteamID() const { return SteamID(steamID); }
			void SetsteamID(const SteamID& val) { steamID = val.ConvertToUInt64(); }
		// Static size: 4
		EIntroducerRouting routingType;
		// Static size: 1450
		std::vector<unsigned char> data;
		// Static size: 4
		uint32_t dataLen;

		MsgClientP2PIntroducerMessage()
		{
			steamID = 0;
			routingType = EIntroducerRouting::Unknown;
			data = std::vector<unsigned char>(1450);
			dataLen = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamID);
			stream.Write((int)routingType);
			stream.Write(data);
			stream.Write(dataLen);

		}

		void Deserialize( Stream& stream )
		{
			steamID = (uint64_t)stream.Read<int64_t>();
			routingType = (EIntroducerRouting)stream.Read<int32_t>();
			data = stream.ReadArray<unsigned char>(1450);
			dataLen = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSBeginSession : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientOGSBeginSession; }

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

		void Serialize(Stream& stream)
		{

			stream.Write(accountType);
			stream.Write(accountId);
			stream.Write(appId);
			stream.Write(timeStarted);

		}

		void Deserialize( Stream& stream )
		{
			accountType = (unsigned char)stream.Read<unsigned char>();
			accountId = (uint64_t)stream.Read<int64_t>();
			appId = (uint32_t)stream.Read<int32_t>();
			timeStarted = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSBeginSessionResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientOGSBeginSessionResponse; }

		// Static size: 4
		EResult result;
		// Static size: 1
		unsigned char collectingAny;
		// Static size: 1
		unsigned char collectingDetails;
		// Static size: 8
		uint64_t sessionId;

		MsgClientOGSBeginSessionResponse()
		{
			result = EResult::Invalid;
			collectingAny = 0;
			collectingDetails = 0;
			sessionId = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(collectingAny);
			stream.Write(collectingDetails);
			stream.Write(sessionId);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			collectingAny = (unsigned char)stream.Read<unsigned char>();
			collectingDetails = (unsigned char)stream.Read<unsigned char>();
			sessionId = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientOGSEndSession : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientOGSEndSession; }

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

		void Serialize(Stream& stream)
		{

			stream.Write(sessionId);
			stream.Write(timeEnded);
			stream.Write(reasonCode);
			stream.Write(countAttributes);

		}

		void Deserialize( Stream& stream )
		{
			sessionId = (uint64_t)stream.Read<int64_t>();
			timeEnded = (uint32_t)stream.Read<int32_t>();
			reasonCode = (int32_t)stream.Read<int32_t>();
			countAttributes = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSEndSessionResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientOGSEndSessionResponse; }

		// Static size: 4
		EResult result;

		MsgClientOGSEndSessionResponse()
		{
			result = EResult::Invalid;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientOGSWriteRow : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientOGSWriteRow; }

		// Static size: 8
		uint64_t sessionId;
		// Static size: 4
		int32_t countAttributes;

		MsgClientOGSWriteRow()
		{
			sessionId = 0;
			countAttributes = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(sessionId);
			stream.Write(countAttributes);

		}

		void Deserialize( Stream& stream )
		{
			sessionId = (uint64_t)stream.Read<int64_t>();
			countAttributes = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientGetFriendsWhoPlayGame : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientGetFriendsWhoPlayGame; }

		// Static size: 8
		private:
			uint64_t gameId;
		public:
			GameID GetgameId() const { return GameID(gameId); }
			void SetgameId(const GameID& val) { gameId = val.ToUInt64(); }

		MsgClientGetFriendsWhoPlayGame()
		{
			gameId = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(gameId);

		}

		void Deserialize( Stream& stream )
		{
			gameId = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientGetFriendsWhoPlayGameResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientGetFriendsWhoPlayGameResponse; }

		// Static size: 4
		EResult result;
		// Static size: 8
		private:
			uint64_t gameId;
		public:
			GameID GetgameId() const { return GameID(gameId); }
			void SetgameId(const GameID& val) { gameId = val.ToUInt64(); }
		// Static size: 4
		uint32_t countFriends;

		MsgClientGetFriendsWhoPlayGameResponse()
		{
			result = EResult::Invalid;
			gameId = 0;
			countFriends = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(gameId);
			stream.Write(countFriends);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			gameId = (uint64_t)stream.Read<int64_t>();
			countFriends = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSPerformHardwareSurvey : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSPerformHardwareSurvey; }

		// Static size: 4
		uint32_t flags;

		MsgGSPerformHardwareSurvey()
		{
			flags = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(flags);

		}

		void Deserialize( Stream& stream )
		{
			flags = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSGetPlayStatsResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSGetPlayStatsResponse; }

		// Static size: 4
		EResult result;
		// Static size: 4
		int32_t rank;
		// Static size: 4
		uint32_t lifetimeConnects;
		// Static size: 4
		uint32_t lifetimeMinutesPlayed;

		MsgGSGetPlayStatsResponse()
		{
			result = EResult::Invalid;
			rank = 0;
			lifetimeConnects = 0;
			lifetimeMinutesPlayed = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(rank);
			stream.Write(lifetimeConnects);
			stream.Write(lifetimeMinutesPlayed);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			rank = (int32_t)stream.Read<int32_t>();
			lifetimeConnects = (uint32_t)stream.Read<int32_t>();
			lifetimeMinutesPlayed = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSGetReputationResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSGetReputationResponse; }

		// Static size: 4
		EResult result;
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
			result = EResult::Invalid;
			reputationScore = 0;
			banned = 0;
			bannedIp = 0;
			bannedPort = 0;
			bannedGameId = 0;
			timeBanExpires = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(reputationScore);
			stream.Write(banned);
			stream.Write(bannedIp);
			stream.Write(bannedPort);
			stream.Write(bannedGameId);
			stream.Write(timeBanExpires);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
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
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSDeny; }

		// Static size: 8
		private:
			uint64_t steamId;
		public:
			SteamID GetsteamId() const { return SteamID(steamId); }
			void SetsteamId(const SteamID& val) { steamId = val.ConvertToUInt64(); }
		// Static size: 4
		EDenyReason denyReason;

		MsgGSDeny()
		{
			steamId = 0;
			denyReason = EDenyReason::Unknown;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamId);
			stream.Write((int)denyReason);

		}

		void Deserialize( Stream& stream )
		{
			steamId = (uint64_t)stream.Read<int64_t>();
			denyReason = (EDenyReason)stream.Read<int32_t>();
		}
	};

	struct MsgGSApprove : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSApprove; }

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

		void Serialize(Stream& stream)
		{

			stream.Write(steamId);

		}

		void Deserialize( Stream& stream )
		{
			steamId = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgGSKick : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSKick; }

		// Static size: 8
		private:
			uint64_t steamId;
		public:
			SteamID GetsteamId() const { return SteamID(steamId); }
			void SetsteamId(const SteamID& val) { steamId = val.ConvertToUInt64(); }
		// Static size: 4
		EDenyReason denyReason;
		// Static size: 4
		int32_t waitTilMapChange;

		MsgGSKick()
		{
			steamId = 0;
			denyReason = EDenyReason::Unknown;
			waitTilMapChange = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamId);
			stream.Write((int)denyReason);
			stream.Write(waitTilMapChange);

		}

		void Deserialize( Stream& stream )
		{
			steamId = (uint64_t)stream.Read<int64_t>();
			denyReason = (EDenyReason)stream.Read<int32_t>();
			waitTilMapChange = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgGSGetUserGroupStatus : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSGetUserGroupStatus; }

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

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdUser);
			stream.Write(steamIdGroup);

		}

		void Deserialize( Stream& stream )
		{
			steamIdUser = (uint64_t)stream.Read<int64_t>();
			steamIdGroup = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgGSGetUserGroupStatusResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::GSGetUserGroupStatusResponse; }

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
		EClanRelationship clanRelationship;
		// Static size: 4
		EClanRank clanRank;

		MsgGSGetUserGroupStatusResponse()
		{
			steamIdUser = 0;
			steamIdGroup = 0;
			clanRelationship = EClanRelationship::None;
			clanRank = EClanRank::None;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdUser);
			stream.Write(steamIdGroup);
			stream.Write((int)clanRelationship);
			stream.Write((int)clanRank);

		}

		void Deserialize( Stream& stream )
		{
			steamIdUser = (uint64_t)stream.Read<int64_t>();
			steamIdGroup = (uint64_t)stream.Read<int64_t>();
			clanRelationship = (EClanRelationship)stream.Read<int32_t>();
			clanRank = (EClanRank)stream.Read<int32_t>();
		}
	};

	struct MsgClientJoinChat : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientJoinChat; }

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

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdChat);
			stream.Write(isVoiceSpeaker);

		}

		void Deserialize( Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			isVoiceSpeaker = (unsigned char)stream.Read<unsigned char>();
		}
	};

	struct MsgClientChatEnter : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientChatEnter; }

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
		EChatRoomType chatRoomType;
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
		EChatRoomEnterResponse enterResponse;
		// Static size: 4
		int32_t numMembers;

		MsgClientChatEnter()
		{
			steamIdChat = 0;
			steamIdFriend = 0;
			chatRoomType = EChatRoomType::Unknown;
			steamIdOwner = 0;
			steamIdClan = 0;
			chatFlags = 0;
			enterResponse = EChatRoomEnterResponse::Unknown;
			numMembers = 0;
		}

		void Serialize(Stream& stream)
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

		void Deserialize( Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			steamIdFriend = (uint64_t)stream.Read<int64_t>();
			chatRoomType = (EChatRoomType)stream.Read<int32_t>();
			steamIdOwner = (uint64_t)stream.Read<int64_t>();
			steamIdClan = (uint64_t)stream.Read<int64_t>();
			chatFlags = (unsigned char)stream.Read<unsigned char>();
			enterResponse = (EChatRoomEnterResponse)stream.Read<int32_t>();
			numMembers = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatMsg : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientChatMsg; }

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
		EChatEntryType chatMsgType;

		MsgClientChatMsg()
		{
			steamIdChatter = 0;
			steamIdChatRoom = 0;
			chatMsgType = EChatEntryType::Invalid;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdChatter);
			stream.Write(steamIdChatRoom);
			stream.Write((int)chatMsgType);

		}

		void Deserialize( Stream& stream )
		{
			steamIdChatter = (uint64_t)stream.Read<int64_t>();
			steamIdChatRoom = (uint64_t)stream.Read<int64_t>();
			chatMsgType = (EChatEntryType)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatMemberInfo : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientChatMemberInfo; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 4
		EChatInfoType type;

		MsgClientChatMemberInfo()
		{
			steamIdChat = 0;
			type = EChatInfoType::Unknown;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdChat);
			stream.Write((int)type);

		}

		void Deserialize( Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			type = (EChatInfoType)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatAction : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientChatAction; }

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
		EChatAction chatAction;

		MsgClientChatAction()
		{
			steamIdChat = 0;
			steamIdUserToActOn = 0;
			chatAction = EChatAction::Unknown;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdChat);
			stream.Write(steamIdUserToActOn);
			stream.Write((int)chatAction);

		}

		void Deserialize( Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			steamIdUserToActOn = (uint64_t)stream.Read<int64_t>();
			chatAction = (EChatAction)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatActionResult : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientChatActionResult; }

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
		EChatAction chatAction;
		// Static size: 4
		EChatActionResult actionResult;

		MsgClientChatActionResult()
		{
			steamIdChat = 0;
			steamIdUserActedOn = 0;
			chatAction = EChatAction::Unknown;
			actionResult = EChatActionResult::Unknown;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdChat);
			stream.Write(steamIdUserActedOn);
			stream.Write((int)chatAction);
			stream.Write((int)actionResult);

		}

		void Deserialize( Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			steamIdUserActedOn = (uint64_t)stream.Read<int64_t>();
			chatAction = (EChatAction)stream.Read<int32_t>();
			actionResult = (EChatActionResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientChatRoomInfo : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientChatRoomInfo; }

		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 4
		EChatInfoType type;

		MsgClientChatRoomInfo()
		{
			steamIdChat = 0;
			type = EChatInfoType::Unknown;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(steamIdChat);
			stream.Write((int)type);

		}

		void Deserialize( Stream& stream )
		{
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			type = (EChatInfoType)stream.Read<int32_t>();
		}
	};

	struct MsgClientSetIgnoreFriend : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientSetIgnoreFriend; }

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

		void Serialize(Stream& stream)
		{

			stream.Write(mySteamId);
			stream.Write(steamIdFriend);
			stream.Write(ignore);

		}

		void Deserialize( Stream& stream )
		{
			mySteamId = (uint64_t)stream.Read<int64_t>();
			steamIdFriend = (uint64_t)stream.Read<int64_t>();
			ignore = (unsigned char)stream.Read<unsigned char>();
		}
	};

	struct MsgClientSetIgnoreFriendResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientSetIgnoreFriendResponse; }

		// Static size: 8
		private:
			uint64_t friendId;
		public:
			SteamID GetfriendId() const { return SteamID(friendId); }
			void SetfriendId(const SteamID& val) { friendId = val.ConvertToUInt64(); }
		// Static size: 4
		EResult result;

		MsgClientSetIgnoreFriendResponse()
		{
			friendId = 0;
			result = EResult::Invalid;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(friendId);
			stream.Write((int)result);

		}

		void Deserialize( Stream& stream )
		{
			friendId = (uint64_t)stream.Read<int64_t>();
			result = (EResult)stream.Read<int32_t>();
		}
	};

	struct MsgClientLoggedOff : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientLoggedOff; }

		// Static size: 4
		EResult result;
		// Static size: 4
		int32_t secMinReconnectHint;
		// Static size: 4
		int32_t secMaxReconnectHint;

		MsgClientLoggedOff()
		{
			result = EResult::Invalid;
			secMinReconnectHint = 0;
			secMaxReconnectHint = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(secMinReconnectHint);
			stream.Write(secMaxReconnectHint);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			secMinReconnectHint = (int32_t)stream.Read<int32_t>();
			secMaxReconnectHint = (int32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientLogOnResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientLogOnResponse; }

		// Static size: 4
		EResult result;
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
			result = EResult::Invalid;
			outOfGameHeartbeatRateSec = 0;
			inGameHeartbeatRateSec = 0;
			clientSuppliedSteamId = 0;
			ipPublic = 0;
			serverRealTime = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(outOfGameHeartbeatRateSec);
			stream.Write(inGameHeartbeatRateSec);
			stream.Write(clientSuppliedSteamId);
			stream.Write(ipPublic);
			stream.Write(serverRealTime);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			outOfGameHeartbeatRateSec = (int32_t)stream.Read<int32_t>();
			inGameHeartbeatRateSec = (int32_t)stream.Read<int32_t>();
			clientSuppliedSteamId = (uint64_t)stream.Read<int64_t>();
			ipPublic = (uint32_t)stream.Read<int32_t>();
			serverRealTime = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientServerUnavailable : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientServerUnavailable; }

		// Static size: 8
		uint64_t jobidSent;
		// Static size: 4
		uint32_t eMsgSent;
		// Static size: 4
		EServerType eServerTypeUnavailable;

		MsgClientServerUnavailable()
		{
			jobidSent = 0;
			eMsgSent = 0;
			eServerTypeUnavailable = EServerType::First;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(jobidSent);
			stream.Write(eMsgSent);
			stream.Write((int)eServerTypeUnavailable);

		}

		void Deserialize( Stream& stream )
		{
			jobidSent = (uint64_t)stream.Read<int64_t>();
			eMsgSent = (uint32_t)stream.Read<int32_t>();
			eServerTypeUnavailable = (EServerType)stream.Read<int32_t>();
		}
	};

	struct MsgClientCreateChat : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientCreateChat; }

		// Static size: 4
		EChatRoomType chatRoomType;
		// Static size: 8
		private:
			uint64_t gameId;
		public:
			GameID GetgameId() const { return GameID(gameId); }
			void SetgameId(const GameID& val) { gameId = val.ToUInt64(); }
		// Static size: 8
		private:
			uint64_t steamIdClan;
		public:
			SteamID GetsteamIdClan() const { return SteamID(steamIdClan); }
			void SetsteamIdClan(const SteamID& val) { steamIdClan = val.ConvertToUInt64(); }
		// Static size: 4
		EChatPermission permissionOfficer;
		// Static size: 4
		EChatPermission permissionMember;
		// Static size: 4
		EChatPermission permissionAll;
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
			chatRoomType = EChatRoomType::Unknown;
			gameId = 0;
			steamIdClan = 0;
			// These were 0; this may be completely incorrect now.
			permissionOfficer = EChatPermission::OfficerDefault;
			permissionMember = EChatPermission::MemberDefault;
			permissionAll = EChatPermission::OwnerDefault;
			// End block
			membersMax = 0;
			chatFlags = 0;
			steamIdFriendChat = 0;
			steamIdInvited = 0;
		}

		void Serialize(Stream& stream)
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

		void Deserialize( Stream& stream )
		{
			chatRoomType = (EChatRoomType)stream.Read<int32_t>();
			gameId = (uint64_t)stream.Read<int64_t>();
			steamIdClan = (uint64_t)stream.Read<int64_t>();
			permissionOfficer = (EChatPermission)stream.Read<int32_t>();
			permissionMember = (EChatPermission)stream.Read<int32_t>();
			permissionAll = (EChatPermission)stream.Read<int32_t>();
			membersMax = (uint32_t)stream.Read<int32_t>();
			chatFlags = (unsigned char)stream.Read<unsigned char>();
			steamIdFriendChat = (uint64_t)stream.Read<int64_t>();
			steamIdInvited = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientCreateChatResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientCreateChatResponse; }

		// Static size: 4
		EResult result;
		// Static size: 8
		private:
			uint64_t steamIdChat;
		public:
			SteamID GetsteamIdChat() const { return SteamID(steamIdChat); }
			void SetsteamIdChat(const SteamID& val) { steamIdChat = val.ConvertToUInt64(); }
		// Static size: 4
		EChatRoomType chatRoomType;
		// Static size: 8
		private:
			uint64_t steamIdFriendChat;
		public:
			SteamID GetsteamIdFriendChat() const { return SteamID(steamIdFriendChat); }
			void SetsteamIdFriendChat(const SteamID& val) { steamIdFriendChat = val.ConvertToUInt64(); }

		MsgClientCreateChatResponse()
		{
			result = EResult::Invalid;
			steamIdChat = 0;
			chatRoomType = EChatRoomType::Unknown;
			steamIdFriendChat = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write((int)result);
			stream.Write(steamIdChat);
			stream.Write((int)chatRoomType);
			stream.Write(steamIdFriendChat);

		}

		void Deserialize( Stream& stream )
		{
			result = (EResult)stream.Read<int32_t>();
			steamIdChat = (uint64_t)stream.Read<int64_t>();
			chatRoomType = (EChatRoomType)stream.Read<int32_t>();
			steamIdFriendChat = (uint64_t)stream.Read<int64_t>();
		}
	};

	struct MsgClientMarketingMessageUpdate2 : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientMarketingMessageUpdate2; }

		// Static size: 4
		uint32_t marketingMessageUpdateTime;
		// Static size: 4
		uint32_t count;

		MsgClientMarketingMessageUpdate2()
		{
			marketingMessageUpdateTime = 0;
			count = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(marketingMessageUpdateTime);
			stream.Write(count);

		}

		void Deserialize( Stream& stream )
		{
			marketingMessageUpdateTime = (uint32_t)stream.Read<int32_t>();
			count = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientGetLegacyGameKey : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientGetLegacyGameKey; }

		// Static size: 4
		uint32_t appId;

		MsgClientGetLegacyGameKey()
		{
			appId = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(appId);

		}

		void Deserialize( Stream& stream )
		{
			appId = (uint32_t)stream.Read<int32_t>();
		}
	};

	struct MsgClientGetLegacyGameKeyResponse : public ISteamSerializableMessage
	{
		SteamInternal::EMsg GetEMsg() const { return SteamInternal::EMsg::ClientGetLegacyGameKeyResponse; }

		// Static size: 4
		uint32_t appId;
		// Static size: 4
		EResult result;
		// Static size: 4
		uint32_t length;

		MsgClientGetLegacyGameKeyResponse()
		{
			appId = 0;
			result = EResult::Invalid;
			length = 0;
		}

		void Serialize(Stream& stream)
		{

			stream.Write(appId);
			stream.Write((int)result);
			stream.Write(length);

		}

		void Deserialize( Stream& stream )
		{
			appId = (uint32_t)stream.Read<int32_t>();
			result = (EResult)stream.Read<int32_t>();
			length = (uint32_t)stream.Read<int32_t>();
		}
	};

} // namespace SteamInternal::Internal

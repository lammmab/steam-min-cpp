#pragma once

#include <string>
#include <cstdint>
#include "vdf.hpp"

namespace Steam::Messaging::Packets {
    class IPacketMsg;
}

namespace Steam::Events {
    struct Event {
        virtual ~Event() = default;
    };

    struct EventResult {
        bool success;
        std::string message;

        static EventResult ok() {
            return {true, {}};
        }

        static EventResult fail(std::string msg) {
            return {false, std::move(msg)};
        }
    };

    struct ResultEvent : Event {
        EventResult result;

        ResultEvent() = default;
        ResultEvent(EventResult r) : result(std::move(r)) {}

        bool ok() const { return result.success; }
        const std::string& what() const { return result.message; }
    };

    struct ChannelSecuredEvent : ResultEvent {
        using ResultEvent::ResultEvent;
    };

    struct ClientLogonEvent : ResultEvent {
        using ResultEvent::ResultEvent;

        uint64_t steamid;
        int32_t client_sessionid;

        ClientLogonEvent(EventResult r, uint64_t s_id, int32_t c_sid)
            : ResultEvent(std::move(r)),
            steamid(s_id),
            client_sessionid(c_sid) {}
    };

    struct MsgNotImplementedEvent : Event {
        uint32_t emsg;
        const Steam::Messaging::Packets::IPacketMsg* packet; // For error handling purposes; Largely can be ignored.
        explicit MsgNotImplementedEvent(uint32_t e,const Steam::Messaging::Packets::IPacketMsg* p) : emsg(e),packet(p) {}
    };

    struct ProductInfoResult : Event {
        struct ProductAppInfo {
            uint32_t appid;
            std::string sha;
            Steam::Utils::VDF::VDFNode appinfo;
        };

        struct ProductPackageInfo {
            uint32_t packageid;
            std::string sha;
            Steam::Utils::VDF::VDFNode appinfo;
        };

        std::vector<ProductAppInfo> apps;
        std::vector<ProductPackageInfo> packages;

        explicit ProductInfoResult(std::vector<ProductAppInfo> a, std::vector<ProductPackageInfo> p) 
            : apps(std::move(a)),packages(std::move(p)) {}
    };
}

namespace Steam::Commands {
    struct Command {
        virtual ~Command() = default;
    };
    struct AnonymousLogin : Command {};
    struct ClientLogoff : Command {};
    struct GetProductInfo : Command {
        uint32_t id;
        uint64_t steamid;
        int32_t client_sessionid;
        uint64_t app_token = 0;
        
        // Constructor with no app token (limited data)
        explicit GetProductInfo(uint32_t i, uint64_t s_id, int32_t c_sid) 
            : id(i),steamid(s_id),client_sessionid(c_sid) {};

        // Constructor with app token (extended data)
        explicit GetProductInfo(uint32_t i, uint64_t s_id, int32_t c_sid, uint64_t t) 
            : id(i),steamid(s_id),client_sessionid(c_sid),app_token(t) {};
    };
}
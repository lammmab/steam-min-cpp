#pragma once

#include <array>
#include <cstddef>

// Forward declarations
namespace Steam::Messaging {
    class CMClient;

    namespace Packets {
        class PacketClientMsgProtobuf;
        class PacketMsg;
    }
}

namespace Steam::Dispatch {
    using ProtoHandlerFn =
        void(*)(Steam::Messaging::CMClient&, const Steam::Messaging::Packets::PacketClientMsgProtobuf&);

    using MsgHandlerFn =
        void(*)(Steam::Messaging::CMClient&, const Steam::Messaging::Packets::PacketMsg&);

    constexpr size_t MAX_EMSG = 12301;

    struct DispatchTable {
        std::array<ProtoHandlerFn, MAX_EMSG> proto{};
        std::array<MsgHandlerFn, MAX_EMSG> msg{};

        DispatchTable() {
            proto.fill(nullptr);
            msg.fill(nullptr);
        }
    };

    inline DispatchTable g_dispatch;
}
/// @internal
/// \cond
#pragma once

#include <steamclient/routing/events/table.hpp>
#include <steamclient/types/generated/SteamLanguage.hpp>

namespace Steam::Dispatch {

template <Steam::Internal::Enums::EMsg Msg, auto Fn>
struct ProtoRegister {
  ProtoRegister() { g_dispatch.proto[static_cast<uint32_t>(Msg)] = Fn; }
};

template <Steam::Internal::Enums::EMsg Msg, auto Fn>
struct MsgRegister {
  MsgRegister() { g_dispatch.msg[static_cast<uint32_t>(Msg)] = Fn; }
};

}  // namespace Steam::Dispatch
/// \endcond
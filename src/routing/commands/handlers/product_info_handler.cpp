#include <steamclient/network/cmclient.hpp>
#include <steamclient/routing/commands/register.hpp>

#include "steammessages_clientserver_appinfo.pb.h"

using namespace Steam::Messaging;
namespace Enums = Steam::Internal::Enums;
namespace Commands = Steam::Commands;

/*
If GetProductInfo is extended, it can include a vector of app IDs and package
IDs. This will allow for batch results; but I don't need it personally. If you
would like to add it; be my guest.
*/
static void exec_get_product_info(CMClient& client,
                                  const Commands::GetProductInfo& req) {
  ClientMessages::MsgProto<CMsgClientPICSProductInfoRequest> msg(
      Enums::EMsg::ClientPICSProductInfoRequest);

  auto* app = msg.Body.add_apps();
  app->set_appid(req.id);
  app->set_access_token(req.app_token);

  msg.SetSourceJobID(JobIDs::generate_jobid());

  // Later, to reduce boilerplate, we will want to do this automatically.
  // Possibly a helper constructor for ProtoMsg ?
  msg.Header.proto.set_client_sessionid(req.client_sessionid);
  msg.Header.proto.set_steamid(req.steamid);

  client.send_msg(msg);
}

static Steam::Dispatch::RequestRegister<Commands::GetProductInfo,
                                        exec_get_product_info>
    reg_prod_info;
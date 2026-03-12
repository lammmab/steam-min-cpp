#include "network/cmclient.hpp"
#include "base/clientmsg.hpp"
#include <string>

namespace Steam::Authentication {
    class Auth {
    public:
        bool anonymous_login(Steam::Messaging::CMClient& client);
        void logout(Steam::Messaging::CMClient& client);
        inline bool logged_in() const {
            return logged_in_;
        }
    private:
        std::string username_;
        std::string access_token_;
        bool logged_in_ = false;
    };
}
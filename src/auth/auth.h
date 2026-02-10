#include <string>

class Auth {
public:
    bool anonymous_login();
    void logout();
private:
    std::string username_;
    std::string access_token_;
    bool logged_in_;
};
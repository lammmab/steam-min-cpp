#include <string>

class Auth {
public:
    bool anonymous_login();
    void logout();
    inline bool logged_in() const {
        return logged_in_;
    }
private:
    std::string username_;
    std::string access_token_;
    bool logged_in_ = false;
};
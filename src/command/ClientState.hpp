#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP

#include <string>
#include <vector>

struct ClientState {
    std::string nickname;
    std::string username;
    std::string realname;
    bool registered = false;
    std::vector<std::string> channels;
};

#endif // CLIENT_STATE_HPP

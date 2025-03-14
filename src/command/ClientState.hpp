#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP
#include <string>
#include <vector>

struct ClientState {
    std::string nickname;
    std::string username;
    std::string realname;
    bool registered = false;
    bool passwordProvided = true;
    bool operatorStatus = false;
    std::vector<std::string> channels;
    
    bool isRegistered() const 
	{
        return registered;
    }
    bool isOperator() const 
	{
        return operatorStatus;
    }
    bool canCompleteRegistration() const 
	{
        return !nickname.empty() && !username.empty() && passwordProvided && !registered;
    }
};
#endif // CLIENT_STATE_HPP

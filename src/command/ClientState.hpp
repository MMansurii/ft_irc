#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP
#include <string>
#include <vector>

struct ClientState {
    std::string nickname;
    std::string username;
    std::string realname;
    bool passwordProvided;
    bool registered;
    bool operatorStatus;
    std::vector<std::string> channels;
    
	ClientState() : passwordProvided(false), registered(false), operatorStatus(false){}
    
	static ClientState& getInstance() 
	{
        static ClientState instance;
        return instance;
    }

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
#endif 

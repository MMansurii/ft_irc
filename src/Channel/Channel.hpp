#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <iostream>
#include <list>
#include "Client.hpp"

struct ChannelInfo {
    std::string channelName;      
    std::string channelTopic;     
    std::string accessKey;        
    int keyRequired;              // (1 if key is required, 0 otherwise)
    int inviteOnly;               // (1 if invite-only, 0 otherwise)
    int topicRestricted;          // (1 if topic is restricted, 0 otherwise)
    unsigned int maxClients;        
    unsigned int currentClientCount; 
    std::string lastTopicChangeTime; 
    std::string topicSetter;      
    std::string creationTimestamp; 

    // Constructor to initialize default values
    ChannelInfo()
        : keyRequired(0), inviteOnly(0), topicRestricted(0), maxClients(0), currentClientCount(0) {}
};


class Channel
{
    private:
        ChannelInfo info;
        std::list<Client*> clientsInChannel;        // Set of clients in the channel
        std::list<Client*> operatorsInChannel;    // Set of operators in the channel
        std::list<Client*> invitedClients;          // Set of invited clients in the channel

    public:
        Channel(const std::string& name, const std::string& key, Client* client);
};



#endif
#include "Channel.hpp"

Channel::Channel(const std::string& name, const std::string& key, Client* client) {
    info.channelName = name;                   
    info.accessKey = key;                       
    info.keyRequired = !key.empty() ? 1 : 0;    
    info.currentClientCount = 1;                  
    info.maxClients = 100;                        
    info.creationTimestamp = "2025-03-14";      
    clientsInChannel.push_back(client);                      
    operatorsInChannel.push_back(client);                  
}

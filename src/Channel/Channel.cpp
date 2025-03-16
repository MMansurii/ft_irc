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

Channel::~Channel(){}

std::string Channel::getChannelDetail(ChannelDetailType type) const {
    switch (type) {
        case CHANNEL_NAME:
            return info.channelName;
        case CHANNEL_TOPIC:
            return info.channelTopic;
        case TOPIC_MODE:
            return info.topicRestricted ? "Enabled" : "Disabled";  // Assuming topicMode is represented by topicRestricted
        case MAX_CLIENTS:
            return std::to_string(info.maxClients);  // Convert integer to string
        case CURRENT_CLIENT_COUNT:
            return std::to_string(info.currentClientCount);  // Convert integer to string
        case TOPIC_TIME:
            return info.lastTopicChangeTime;
        case TOPIC_SETTER:
            return info.topicSetter;
        case CREATION_DATE:
            return info.creationTimestamp;
        default:
            return "Unknown";  // Default case if the type is unknown
    }
}
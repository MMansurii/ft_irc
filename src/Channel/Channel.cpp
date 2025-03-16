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

//GETTER

std::string Channel::getChannelDetail(ChannelDetailType type) const {
    switch (type) {
        case CHANNEL_NAME:
            return info.channelName;
        case CHANNEL_TOPIC:
            return info.channelTopic;
        case TOPIC_MODE:
            return std::to_string(info.topicRestricted); //? "Enabled" : "Disabled";  
        case MAX_CLIENTS:
            return std::to_string(info.maxClients);  
        case CURRENT_CLIENT_COUNT:
            return std::to_string(info.currentClientCount);  
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

//SETTER

void Channel::setChannelInfo(ChannelDetailType detailType, const std::string& value, int intValue ) {
    switch (detailType) {
        case CHANNEL_NAME:
            info.channelName = value;
            break;
        case CHANNEL_TOPIC:
            info.channelTopic = value;
            break;
        case TOPIC_MODE:
            info.topicRestricted = intValue;
            break;
        case MAX_CLIENTS:
            info.maxClients = intValue;
            break;
        case CURRENT_CLIENT_COUNT:
            info.currentClientCount += intValue;
            break;
        case TOPIC_TIME:
            info.lastTopicChangeTime = value;
            break;
        case TOPIC_SETTER:
            info.topicSetter = value;
            break;
        case CREATION_DATE:
            info.creationTimestamp = value;
            break;
        default:
            std::cerr << "Invalid ChannelDetailType" << std::endl;
    }
}

Client *Channel::findOperatorByNickname(const std::string &nickname) {
    for (auto& op : operatorsInChannel) {
        if (op->getCl_str_info(1) == nickname) {
            return op;
        }
    }
    return nullptr; // Return nullptr if not found
}
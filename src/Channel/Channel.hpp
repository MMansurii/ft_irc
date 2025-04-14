#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <iostream>
#include <list>
#include "Client.hpp"

enum ChannelDetailType {
    CHANNEL_NAME,
    CHANNEL_TOPIC,
    TOPIC_MODE,
    MAX_CLIENTS,
    CURRENT_CLIENT_COUNT,
    TOPIC_TIME,
    TOPIC_SETTER,
    CREATION_DATE,
    ACCESS_KEY,
    
};

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

};


class Channel
{
    private:
        ChannelInfo info;
        std::list<Client*> clientsInChannel;        // Set of clients in the channel
        std::list<Client*> operatorsInChannel;    // Set of operators in the channel
        std::list<Client*> invitedClients;          // Set of invited clients in the channel
        Channel();
		Channel(Channel const &src);
		Channel& operator=(Channel const &src);

    public:
        Channel(const std::string &name, const std::string &key, Client *client);
        ~Channel();
        std::string getChannelDetail(ChannelDetailType type) const;
        void setChannelInfo(ChannelDetailType detailType, const std::string &value, int intValue);
        Client *findOperatorByNickname(const std::string &nickname);
        Client *findClientByNickname(const std::string &nickname);

        int isUserInChannel(const std::string &nickname);
        int isOperatorInChannel(const std::string &nickname);
        int isClientInvited(const std::string &nickname);

        void addInvitedGuest(Client *newGuest);
        void sendUserListToClient(Client *client);
        std::string attemptJoinChannel( Client *client, const std::string &providedKey);
        void removeInvitedClient(const std::string &clientNickname);
        void removeClientsInChannel(const std::string& clientNickname);
        void removeOperatorsInChannel(const std::string& clientNickname);

        std::string getChannelModes() const;
        void broadcastMessage(Client *sender, const std::string &message);

};      



#endif
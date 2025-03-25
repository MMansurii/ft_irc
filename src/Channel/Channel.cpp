#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, Client *client)
{
    info.channelName = name;
    info.accessKey = key;
    info.keyRequired = !key.empty() ? 1 : 0;
    info.currentClientCount = 1;
    info.maxClients = 100;
    info.creationTimestamp = "2025-03-14";
    clientsInChannel.push_back(client);
    operatorsInChannel.push_back(client);
}

Channel::~Channel() {}

// GETTER

std::string Channel::getChannelDetail(ChannelDetailType type) const
{
    switch (type)
    {
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
    // case ACCESS_KEY:
    //     return info.accessKey;

    default:
        return "Unknown";
    }
}

// SETTER

void Channel::setChannelInfo(ChannelDetailType detailType, const std::string &value, int intValue)
{
    switch (detailType)
    {
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

Client *Channel::findOperatorByNickname(const std::string &nickname)
{
    for (auto &op : operatorsInChannel)
    {
        if (op->getCl_str_info(1) == nickname)
        {
            return op;
        }
    }
    return nullptr;
}

Client *Channel::findClientByNickname(const std::string &nickname)
{
    for (auto &client : clientsInChannel)
    {
        if (client->getCl_str_info(1) == nickname)
        {
            return client;
        }
    }
    return nullptr;
}

int Channel::isUserInChannel(const std::string &nickname)
{
    for (auto &client : clientsInChannel)
    {
        if (client->getCl_str_info(1) == nickname)
        {
            return 1;
        }
    }
    return 0;
}

int Channel::isOperatorInChannel(const std::string &nickname)
{
    for (auto &operatorUser : operatorsInChannel)
    {
        if (operatorUser->getCl_str_info(1) == nickname)
        {
            return 1;
        }
    }
    return 0;
}

int Channel::foundInvited(const std::string &nickname)
{
    for (auto &invitedUser : this->invitedClients)
    {
        if (invitedUser->getCl_str_info(1) == nickname)
        {
            return 1;
        }
    }
    return 0;
}

void Channel::addInvitedGuest(Client *newGuest)
{
    this->invitedClients.push_back(newGuest);
}

void Channel::sendUserListToClient(Client *client)
{
    // Reply Code 353 (USER_LIST_REPLY): Server sends a list of users in the channel
    std::string userListMessage = ":" + client->getCl_str_info(4) + " 353 " + client->getCl_str_info(1) + " = " + this->getChannelDetail(CHANNEL_NAME) + " :";

    bool isFirstOperator = true;
    for (auto &operatorUser : this->operatorsInChannel)
    {
        if (!isFirstOperator)
        {
            userListMessage += ' ';
        }
        userListMessage += '@' + operatorUser->getCl_str_info(1); // Prefix operator nicknames with '@'
        isFirstOperator = false;
    }

    bool isFirstUser = true;
    for (auto &regularUser : this->clientsInChannel)
    {
        if (!isFirstUser)
        {
            userListMessage += ' ';
        }
        userListMessage += regularUser->getCl_str_info(1); 
        isFirstUser = false;
    }

    client->do_TMess(userListMessage, 2); 

    // Reply Code 366 (END_OF_USER_LIST_REPLY): Marks the end of the user list
    std::string endOfListMessage = ":" + client->getCl_str_info(4) + " 366 " + client->getCl_str_info(1) + ' ' + this->getChannelDetail(CHANNEL_NAME) + " :End of /NAMES list";
    client->do_TMess(endOfListMessage, 2); 
}

void Channel::removeInvitedUser(const std::string& userNickname)
{
    for (auto iterator = this->invitedClients.begin(); iterator != this->invitedClients.end(); )
    {
        if ((*iterator)->getCl_str_info(1) == userNickname)
        {
            iterator = this->invitedClients.erase(iterator);
            break; 
        }
        else
        {
            ++iterator;
        }
    }
}


std::string Channel::attemptJoinChannel(const std::string &providedKey, Client *client)
{
    if (info.inviteOnly)
    {
        if (!foundInvited(client->getCl_str_info(1)))
        {
            return ("473 " + client->getCl_str_info(1) + ' ' + this->getChannelDetail(CHANNEL_NAME) + " :Cannot join channel (+i)");
        }
    }

    if ((info.maxClients > 0 && info.currentClientCount >= info.maxClients) || info.currentClientCount >= 1000)
    {
        return ("471 " + client->getCl_str_info(1) + ' ' + this->getChannelDetail(CHANNEL_NAME) + " :Cannot join channel (+l)");
    }

    if (!info.keyRequired || (providedKey == info.accessKey))
    {
        this->clientsInChannel.push_back(client);
        this->info.currentClientCount++;

        if (isUserInChannel(client->getCl_str_info(1)))
        {
            removeInvitedUser(client->getCl_str_info(1));
        }

        return ":" + client->getCl_str_info(1) + "!" + client->getCl_str_info(1) + "@" + client->getCl_str_info(2) + " JOIN " + info.channelName;
    }
    else
    {
        return ("475 " + client->getCl_str_info(1) + ' ' + info.channelName + " :Cannot join channel (+k)");
    }
}

std::string Channel::getChannelModes() const
{
    std::string modeString = "+";

    if (info.inviteOnly) modeString += "i";
    if (info.topicRestricted) modeString += "t";
    if (info.keyRequired) modeString += "k";

    if (info.maxClients > 0)
    {
        modeString += "l " + std::to_string(info.maxClients);
    }

    return (modeString == "+") ? "" : modeString;
}


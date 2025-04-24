#include "Channel.hpp"
#include <set>

Channel::Channel(const std::string &name, const std::string &key, Client *client)
{
    info.channelName = name;
    info.accessKey = key;
    // initialize modes: key, invite-only, topic-restriction
    info.keyRequired = !key.empty() ? 1 : 0;
    // Initialize channel modes: invite-only and topic restriction off by default
    info.inviteOnly = 0;
    info.topicRestricted = 0;
    info.currentClientCount = 1;
    info.maxClients = 100;
    info.creationTimestamp = "2025-03-14";
    // Add creator as channel member and operator
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

Client *Channel::findOperatorByNickname(const std::string &alias)
{
    for (auto &op : operatorsInChannel)
    {
        if (op->getCl_str_info(1) == alias)
        {
            return op;
        }
    }
    return nullptr;
}

Client *Channel::findClientByNickname(const std::string &alias)
{
    for (auto &client : clientsInChannel)
    {
        if (client->getCl_str_info(1) == alias)
        {
            return client;
        }
    }
    return nullptr;
}

int Channel::isClientInChannel(const std::string &alias)
{
    for (auto &client : clientsInChannel)
    {
        if (client->getCl_str_info(1) == alias)
        {
            return 1;
        }
    }
    return 0;
}

int Channel::isOperatorInChannel(const std::string &alias)
{
    for (auto &operatorClient : operatorsInChannel)
    {
        if (operatorClient->getCl_str_info(1) == alias)
        {
            return 1;
        }
    }
    return 0;
}

int Channel::isClientInvited(const std::string &alias)
{
    for (auto &invitedClient : invitedClients)
    {
        if (invitedClient->getCl_str_info(1) == alias)
        {
            return 1;
        }
    }
    return 0;
}

void Channel::addInvitedGuest(Client *newGuest)
{
    invitedClients.push_back(newGuest);
}

void Channel::sendClientListToClient(Client *client)
{
    // Reply Code 353 (USER_LIST_REPLY): Server sends a list of clients in the channel
    std::string clientListMessage = ":" + client->getCl_str_info(4) + " 353 " + client->getCl_str_info(1) + " = " + getChannelDetail(CHANNEL_NAME) + " :";

    bool isFirstOperator = true;
    for (auto &operatorClient : operatorsInChannel)
    {
        if (!isFirstOperator)
        {
            clientListMessage += ' ';
        }
        clientListMessage += '@' + operatorClient->getCl_str_info(1); // Prefix operator nicknames with '@'
        isFirstOperator = false;
    }

    bool isFirstClient = true;
    for (auto &regularClient : clientsInChannel)
    {
        if (!isFirstClient)
        {
            clientListMessage += ' ';
        }
        clientListMessage += regularClient->getCl_str_info(1);
        isFirstClient = false;
    }

    client->do_TMess(clientListMessage, 2);

    // Reply Code 366 (END_OF_USER_LIST_REPLY): Marks the end of the client list
    std::string endOfListMessage = ":" + client->getCl_str_info(4) + " 366 " + client->getCl_str_info(1) + ' ' + getChannelDetail(CHANNEL_NAME) + " :End of /NAMES list";
    client->do_TMess(endOfListMessage, 2);
}

// Send the current topic (or a "no topic" reply) to a joining client
void Channel::sendTopicToClient(Client *client)
{
    const std::string &server = client->getCl_str_info(4);
    const std::string &alias = client->getCl_str_info(1);
    if (info.channelTopic.empty())
    {
        // RPL_NOTOPIC
        client->do_TMess(":" + server + " 331 " + alias + " " + info.channelName + " :No topic is set", 2);
    }
    else
    {
        // RPL_TOPIC
        client->do_TMess(":" + server + " 332 " + alias + " " + info.channelName + " :" + info.channelTopic, 2);
        // RPL_TOPICWHOTIME
        client->do_TMess(":" + server + " 333 " + alias + " " + info.channelName + " " + info.topicSetter + " " + info.lastTopicChangeTime, 2);
    }
}

void Channel::removeInvitedClient(const std::string &clientNickname)
{
    for (auto iterator = invitedClients.begin(); iterator != invitedClients.end();)
    {
        if ((*iterator)->getCl_str_info(1) == clientNickname)
        {
            iterator = invitedClients.erase(iterator);
            break;
        }
        else
        {
            ++iterator;
        }
    }
}

void Channel::removeClientsInChannel(const std::string &clientNickname)
{
    for (auto iterator = clientsInChannel.begin(); iterator != clientsInChannel.end();)
    {
        if ((*iterator)->getCl_str_info(1) == clientNickname)
        {
            iterator = clientsInChannel.erase(iterator);
            break;
        }
        else
        {
            ++iterator;
        }
    }
}

void Channel::removeOperatorsInChannel(const std::string &clientNickname)
{
    for (auto iterator = operatorsInChannel.begin(); iterator != operatorsInChannel.end();)
    {
        if ((*iterator)->getCl_str_info(1) == clientNickname)
        {
            iterator = operatorsInChannel.erase(iterator);
            break;
        }
        else
        {
            ++iterator;
        }
    }
}

std::string Channel::attemptJoinChannel(Client *client, const std::string &providedKey)
{
    if (info.inviteOnly)
    {
        if (!isClientInvited(client->getCl_str_info(1)))
        {
            return ("473 " + client->getCl_str_info(1) + ' ' + getChannelDetail(CHANNEL_NAME) + " :Cannot join channel (+i)");
        }
    }

    if ((info.maxClients > 0 && info.currentClientCount >= info.maxClients) || info.currentClientCount >= 1000)
    {
        return ("471 " + client->getCl_str_info(1) + ' ' + getChannelDetail(CHANNEL_NAME) + " :Cannot join channel (+l)");
    }

    if (!info.keyRequired || (providedKey == info.accessKey))
    {
        clientsInChannel.push_back(client);
        info.currentClientCount++;

        if (isClientInChannel(client->getCl_str_info(1)))
        {
            removeInvitedClient(client->getCl_str_info(1));
        }

        return ":" + client->getCl_str_info(1) + "!" + client->getCl_str_info(1) + "@" + client->getCl_str_info(2) + " JOIN " + info.channelName;
    }
    else
    {
        return ("475 " + client->getCl_str_info(1) + ' ' + info.channelName + " :Cannot join channel (+k)");
    }
}

// void Channel::broadcastMessage(Client *sender, const std::string &message)
// {
//     for (auto client : operatorsInChannel)
//     {
//         if (client != sender)
//             client->do_TMess(message, 2);
//     }

//     for (auto client : clientsInChannel)
//     {
//         if (client != sender)
//             client->do_TMess(message, 2);
//     }
// }


// void Channel::broadcastMessage(Client *sender, const std::string &message)
// {
//     // Send message to all unique channel members (operators and regular), excluding sender
//     std::set<Client*> sent;
//     for (auto client : operatorsInChannel)
//     {
//         if (client != sender)
//         {
//             client->do_TMess(message, 2);
//             sent.insert(client);
//         }
//     }
//     for (auto client : clientsInChannel)
//     {
//         if (client != sender && sent.find(client) == sent.end())
//         {
//             client->do_TMess(message, 2);
//             sent.insert(client);
//         }
//     }
// }

void Channel::broadcastMessage(Client *sender, const std::string &message)
{
    // Collect unique recipients (excluding sender)
    std::set<Client *> recipients;
    for (auto client : operatorsInChannel)
    {
        if (client != sender)
            recipients.insert(client);
    }
    for (auto client : clientsInChannel)
    {
        if (client != sender)
            recipients.insert(client);
    }
    for (auto client : recipients)
    {
        client->do_TMess(message, 2);
    }
}

// std::string Channel::getChannelModes() const
// {
//     std::string modeString = "+";

//     if (info.inviteOnly)
//         modeString += "i";
//     if (info.topicRestricted)
//         modeString += "t";
//     if (info.keyRequired)
//         modeString += "k";

//     if (info.maxClients > 0)
//     {
//         modeString += "l " + std::to_string(info.maxClients);
//     }

//     return (modeString == "+") ? "" : modeString;
// }

std::string Channel::getChannelModes() const
{
    std::ostringstream oss;
    std::string flags;

    if (info.inviteOnly == 1)
        flags += 'i';
    if (info.topicRestricted == 1)
        flags += 't';
    if (info.keyRequired == 1)
        flags += 'k';
    if (info.maxClients != 0)
    {
        flags += 'l';
        oss << " " << info.maxClients;
    }

    if (flags.empty())
        return "";

    return "+" + flags + oss.str();
}


// void Channel::handleKickCommand(Client *requester, const std::string &target, const std::string &comment)
// {
//     for (auto cl = clientsInChannel.begin(); cl != clientsInChannel.end(); ++cl)
//     {
//         if ((*cl)->getCl_str_info(1) == target)
//         {
//             std::string kickMsg = ":" + requester->getCl_str_info(1) + " KICK " + info.channelName + ' ' + target + " :" + comment;

//             broadcastMessage(requester, kickMsg); // Notify all others
//             requester->do_TMess(kickMsg, 2);            // Confirm to the one who issued the KICK

//             clientsInChannel.erase(cl); // Remove from channel
//             --info.currentClientCount;  // Decrement client count
//             break;
//         }
//     }
// }

void Channel::handleKickCommand(Client *requester, const std::string &target, const std::string &comment)
{
    for (auto it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it)
    {
        if ((*it)->getCl_str_info(1) == target)
        {
            // Format KICK message with full client prefix
            std::string kickMsg = ":" + requester->getCl_str_info(1) + "!" + requester->getCl_str_info(0) + "@" + requester->getCl_str_info(2) + " KICK " + info.channelName + " " + target + " :" + comment;

            broadcastMessage(requester, kickMsg); // Notify all others
            requester->do_TMess(kickMsg, 2);            // Confirm to the one who issued the KICK

            clientsInChannel.erase(it); // Remove from channel
            --info.currentClientCount;  // Decrement client count
            // Also remove operator status if target was an operator
            removeOperatorsInChannel(target);
            break;
        }
    }
}

void Channel::broadcastTopicUpdate(Client *client, const std::string &newTopic)
{
    const std::string &alias = client->getCl_str_info(0);
    const std::string &server = client->getCl_str_info(4);

    info.channelTopic = (newTopic == ":") ? "" : newTopic;
    info.lastTopicChangeTime = std::to_string(time(0));
    info.topicSetter = alias;

    std::string formatTopicReply = ":" + server + " 332 " + alias + ' ' + info.channelName + " :" + info.channelTopic;
    std::string formatTopicWhoTimeReply = ":" + server + " 333 " + alias + " " + info.channelName + " " + info.topicSetter + " " + info.lastTopicChangeTime;
    ;

    broadcastMessage(client, formatTopicReply);
    broadcastMessage(client, formatTopicWhoTimeReply);
}

// void Channel::updateInviteOnlyMode(Client *client, int flag)
// {
//     if (flag != 1 && flag != -1)
//         return;

//     info.inviteOnly = flag;

//     const std::string &alias = client->getCl_str_info(0);
//     const std::string modeChange = (flag == 1) ? "+i" : "-i";
//     const std::string reply = ":" + alias + " MODE " + info.channelName + " " + modeChange;

//     client->do_TMess(reply, 2);
//     broadcastMessage(client, reply);
// }

void Channel::updateInviteOnlyMode(Client *client, int flag)
{
    if (flag != 1 && flag != -1)
        return;

    // Enable or disable invite-only mode (0 off, 1 on)
    info.inviteOnly = (flag == 1) ? 1 : 0;

    const std::string &alias = client->getCl_str_info(0);
    const std::string modeChange = (flag == 1) ? "+i" : "-i";
    const std::string reply = ":" + alias + " MODE " + info.channelName + " " + modeChange;

    client->do_TMess(reply, 2);
    broadcastMessage(client, reply);
}

// void Channel::updateTopicRestrictionMode(Client *client, int flag)
// {
//     if (flag != 1 && flag != -1)
//         return;

//     info.inviteOnly = flag;

//     const std::string &alias = client->getCl_str_info(0);
//     const std::string modeChange = (flag == 1) ? "+t" : "-t";
//     const std::string reply = ":" + alias + " MODE " + info.channelName + " " + modeChange;

//     client->do_TMess(reply, 2);
//     broadcastMessage(client, reply);
// }

void Channel::updateTopicRestrictionMode(Client *client, int flag)
{
    if (flag != 1 && flag != -1)
        return;

    // Enable or disable topic-restricted mode (0 off, 1 on)
    info.topicRestricted = (flag == 1) ? 1 : 0;

    // use client alias for prefix
    const std::string &alias = client->getCl_str_info(1);
    const std::string modeChange = (flag == 1) ? "+t" : "-t";
    const std::string reply = ":" + alias + " MODE " + info.channelName + " " + modeChange;

    client->do_TMess(reply, 2);
    broadcastMessage(client, reply);
}

void Channel::updateChannelKeyMode(Client *client, const std::string &rawArgs, int argIndex, int flag)
{
    const std::string &alias = client->getCl_str_info(0);
    const std::string modePrefix = ":" + alias + " MODE " + info.channelName + " ";

    if (flag == -1)
    {
        info.keyRequired = 0;
        info.accessKey.clear();
        const std::string reply = modePrefix + "-k";
        client->do_TMess(reply, 2);
        broadcastMessage(client, reply);
        return;
    }

    std::istringstream iss(rawArgs);
    std::string arg;
    for (int i = 0; i <= argIndex && iss >> arg; ++i)
    {
        if (i == argIndex)
        {
            if (arg == "x")
            {
                std::string sendMessage = ":" + client->getCl_str_info(4) + " 525 " + alias + ' ' + info.channelName + " :Channel key cannot be 'x'";
                client->do_TMess(sendMessage, 2);
                return;
            }

            info.keyRequired = 1;
            info.accessKey = arg;

            const std::string reply = modePrefix + "+k";
            client->do_TMess(reply, 2);
            broadcastMessage(client, reply);
            return;
        }
    }
}

void Channel::toggleOperatorStatus(Client *client, const std::string &rawArgs, int argIndex, int flag)
{
    if (isOperatorInChannel(client->getCl_str_info(0)) == 0)
        return;

    std::istringstream iss(rawArgs);
    std::string target;
    for (int i = 0; i <= argIndex && iss >> target; ++i)
    {
        if (i == argIndex)
            break;
    }

    if (target.empty())
        return;

    std::string modeChange;
    if (flag == -1 && isOperatorInChannel(target))
    {
        clientsInChannel.push_back(findOperatorByNickname(target));
        removeOperatorsInChannel(target);
        modeChange = " -o " + target;
    }
    else if (flag == 1 && isOperatorInChannel(target) == 0)
    {
        if (isClientInChannel(target) == 0)
        {
            std::string errorMessage = "441 " + client->getCl_str_info(0) + ' ' + target + ' ' + info.channelName + ":The client is not in this channel";
            client->do_TMess(errorMessage, 2);
            return;
        }
        operatorsInChannel.push_back(findClientByNickname(target));
        removeClientsInChannel(target);
        modeChange = " +o " + target;
    }
    else
    {
        return;
    }

    const std::string prefix = ":" + client->getCl_str_info(0) + " MODE " + info.channelName;
    const std::string reply = prefix + modeChange;

    client->do_TMess(reply, 2);
    broadcastMessage(client, reply);
}

void Channel::updateClientLimit(Client *client, const std::string &rawArgs, int argIndex, int flag)
{
    if (flag == -1)
    {
        info.maxClients = 0;
        std::string msg = ":" + client->getCl_str_info(0) + " MODE " + info.channelName + " -l";
        client->do_TMess(msg, 2);
        broadcastMessage(client, msg);
        return;
    }

    std::istringstream iss(rawArgs);
    std::string token;
    for (int i = 0; i <= argIndex && iss >> token; ++i)
        ;

    if (token.empty())
    {
        std::string noArgError = ":" + client->getCl_str_info(4) + " 400 " + client->getCl_str_info(0) + "!" + client->getCl_str_info(0) + "@" + client->getCl_str_info(2) + ' ' + "MODE" + " :" + "Client limit value is missing.";
        client->do_TMess(noArgError, 2);
        return;
    }

    if (token.find_first_not_of("0123456789") != std::string::npos)
    {
        std::string numericArgError = ":" + client->getCl_str_info(4) + " 400 " + client->getCl_str_info(0) + "!" + client->getCl_str_info(0) + "@" + client->getCl_str_info(2) + ' ' + "MODE" + " :" + "Client limit must be a numeric value.";
        client->do_TMess(numericArgError, 2);
        return;
    }

    int limit = std::stoi(token);
    if (limit > 999)
    {
        std::string maxNumError = ":" + client->getCl_str_info(4) + " 400 " + client->getCl_str_info(0) + "!" + client->getCl_str_info(0) + "@" + client->getCl_str_info(2) + ' ' + "MODE" + " :" + "Client limit cannot exceed 999.";
        client->do_TMess(maxNumError, 2);
        return;
    }

    if (limit < static_cast<int>(info.maxClients))
    {
        std::string limitNumError = ":" + client->getCl_str_info(4) + " 400 " + client->getCl_str_info(0) + "!" + client->getCl_str_info(0) + "@" + client->getCl_str_info(2) + ' ' + "MODE" + " :" + "Limit below current client count.";
        client->do_TMess(limitNumError, 2);

        return;
    }

    info.maxClients = limit;
    std::string msg = ":" + client->getCl_str_info(0) + " MODE " + info.channelName + " +l " + token;
    client->do_TMess(msg, 2);
    broadcastMessage(client, msg);
}

void Channel::displayMemberListToClient(Client *client)
{
    const std::string &server = client->getCl_str_info(4);
    const std::string &alias = client->getCl_str_info(1);

    for (std::list<Client *>::iterator cl = operatorsInChannel.begin(); cl != operatorsInChannel.end(); ++cl)
    {
        std::string message = ":" + server + " 352 " + alias + ' ' + info.channelName + ' ' +
                              (*cl)->getCl_str_info(0) + ' ' + (*cl)->getCl_str_info(2) + ' ' +
                              (*cl)->getCl_str_info(4) + ' ' + (*cl)->getCl_str_info(1) + " @ :0 " + (*cl)->getCl_str_info(3);
        client->do_TMess(message, 2);
    }

    for (std::list<Client *>::iterator cl = clientsInChannel.begin(); cl != clientsInChannel.end(); ++cl)
    {
        std::string message = ":" + server + " 352 " + alias + ' ' + info.channelName + ' ' +
                              (*cl)->getCl_str_info(0) + ' ' + (*cl)->getCl_str_info(2) + ' ' +
                              (*cl)->getCl_str_info(4) + ' ' + (*cl)->getCl_str_info(1) + " H :0 " + (*cl)->getCl_str_info(3);
        client->do_TMess(message, 2);
    }
}

void Channel::applyChannelModes(Client *client, const std::string &modes, std::string modeArgs)
{
    if (validateModeArguments(modes, modeArgs) == 0)
    {
        std::string erorrMessage = ":" + client->getCl_str_info(4) + " 400 " + client->getCl_str_info(0) + "!" + client->getCl_str_info(0) + "@" + client->getCl_str_info(2) + ' ' + "MODE" + " :" + "Invalid mode format.";
        client->do_TMess(erorrMessage, 2);
        return;
    }

    int flag = 0;
    int argIndex = 0;

    for (size_t i = 0; i < modes.length(); ++i)
    {
        char modeChar = modes[i];

        if (modeChar == '+')
            flag = 1;
        else if (modeChar == '-')
            flag = -1;
        else
        {
            switch (modeChar)
            {
            case 'i':
                updateInviteOnlyMode(client, flag);
                break;
            case 't':
                updateTopicRestrictionMode(client, flag);
                break;
            case 'k':
                updateChannelKeyMode(client, modeArgs, argIndex, flag);
                argIndex++;
                break;
            case 'o':
                toggleOperatorStatus(client, modeArgs, argIndex, flag);
                argIndex++;
                break;
            case 'l':
                updateClientLimit(client, modeArgs, argIndex, flag);
                argIndex++;
                break;
            default:

                client->do_TMess(("472 " + client->getCl_str_info(1) + ' ' + std::string(1, modeChar)), 2);
            }
        }
    }
}

// int Channel::validateModeArguments(const std::string &modes, std::string modeArgs)
// {
//     int givenArgs = 0;
//     int expectedArgs = 0;
//     int flag = 0;
//     std::string currentArg;
//     size_t pos;

//     // Count actual arguments provided
//     while (!modeArgs.empty())
//     {
//         pos = modeArgs.find(' ');
//         currentArg = modeArgs.substr(0, pos);
//         modeArgs.erase(0, (pos == std::string::npos) ? currentArg.length() : currentArg.length() + 1);
//         givenArgs++;
//         if (pos == std::string::npos)
//             break;
//     }

//     // Determine expected number of arguments based on mode string
//     if (!modes.empty() && (modes[0] == '+' || modes[0] == '-'))
//     {
//         flag = (modes[0] == '+') ? 1 : -1;
//         for (size_t i = 1; i < modes.size(); ++i)
//         {
//             if (modes[i] == '+')
//                 flag = 1;
//             else if (modes[i] == '-')
//                 flag = -1;
//             else if ((flag == 1 && (modes[i] == 'k' || modes[i] == 'o' || modes[i] == 'l')) ||
//                      (flag == -1 && modes[i] == 'k' && info.keyRequired == 1))
//                 expectedArgs++;
//         }
//     }

//     return (givenArgs == expectedArgs) ? 1 : 0;
// }

int Channel::validateModeArguments(const std::string &modes, std::string modeArgs)
{
    int givenArgs = 0;
    int expectedArgs = 0;
    int flag = 0;
    size_t pos;

    // Count actual arguments provided - converted to for loop
    for (pos = modeArgs.find(' ');
         !modeArgs.empty();
         pos = modeArgs.find(' '))
    {
        std::string currentArg = modeArgs.substr(0, pos);
        modeArgs.erase(0, (pos == std::string::npos) ? currentArg.length() : currentArg.length() + 1);
        givenArgs++;
        if (pos == std::string::npos)
            break;
    }
    
    // Determine expected number of arguments based on mode string
    if (!modes.empty() && (modes[0] == '+' || modes[0] == '-'))
    {
        flag = (modes[0] == '+') ? 1 : -1;
        for (size_t i = 1; i < modes.size(); ++i)
        {
            if (modes[i] == '+')
                flag = 1;
            else if (modes[i] == '-')
                flag = -1;
            else if ((flag == 1 && (modes[i] == 'k' || modes[i] == 'o' || modes[i] == 'l')) ||
                     (flag == -1 && modes[i] == 'k' && info.keyRequired == 1))
                expectedArgs++;
        }
    }

    return (givenArgs == expectedArgs) ? 1 : 0;
}
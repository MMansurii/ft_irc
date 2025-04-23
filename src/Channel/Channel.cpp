#include "Channel.hpp"
// For std::set in broadcastMessage
#include <set>

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

int Channel::isClientInvited(const std::string &nickname)
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

void Channel::removeInvitedClient(const std::string &clientNickname)
{
    for (auto iterator = this->invitedClients.begin(); iterator != this->invitedClients.end();)
    {
        if ((*iterator)->getCl_str_info(1) == clientNickname)
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

void Channel::removeClientsInChannel(const std::string &clientNickname)
{
    for (auto iterator = this->clientsInChannel.begin(); iterator != this->clientsInChannel.end();)
    {
        if ((*iterator)->getCl_str_info(1) == clientNickname)
        {
            iterator = this->clientsInChannel.erase(iterator);
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
    for (auto iterator = this->operatorsInChannel.begin(); iterator != this->operatorsInChannel.end();)
    {
        if ((*iterator)->getCl_str_info(1) == clientNickname)
        {
            iterator = this->operatorsInChannel.erase(iterator);
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
            removeInvitedClient(client->getCl_str_info(1));
        }

        return ":" + client->getCl_str_info(1) + "!" + client->getCl_str_info(1) + "@" + client->getCl_str_info(2) + " JOIN " + info.channelName;
    }
    else
    {
        return ("475 " + client->getCl_str_info(1) + ' ' + info.channelName + " :Cannot join channel (+k)");
    }
}


void Channel::broadcastMessage(Client *sender, const std::string &message)
{
    // Collect unique recipients (excluding sender)
    std::set<Client*> recipients;
    for (auto client : this->operatorsInChannel)
    {
        if (client != sender)
            recipients.insert(client);
    }
    for (auto client : this->clientsInChannel)
    {
        if (client != sender)
            recipients.insert(client);
    }
    for (auto client : recipients)
    {
        client->do_TMess(message, 2);
    }
}

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

void Channel::handleKickCommand(Client *requester, const std::string &target, const std::string &comment)
{
    for (auto it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it)
    {
        if ((*it)->getCl_str_info(1) == target)
        {
            std::string kickMsg = ":" + requester->getCl_str_info(1) + " KICK " + info.channelName + ' ' + target + " :" + comment;

            this->broadcastMessage(requester, kickMsg); // Notify all others
            requester->do_TMess(kickMsg, 2);            // Confirm to the one who issued the KICK

            clientsInChannel.erase(it);        // Remove from channel members
            removeOperatorsInChannel(target);   // Remove operator status if any
            --info.currentClientCount;         // Decrement member count
            break;
        }
    }
}

void Channel::broadcastTopicUpdate(Client *client, const std::string &newTopic)
{
    const std::string &nickname = client->getCl_str_info(0);
    const std::string &server = client->getCl_str_info(4);

    info.channelTopic = (newTopic == ":") ? "" : newTopic;
    info.lastTopicChangeTime = std::to_string(time(0));
    info.topicSetter = nickname;

    std::string formatTopicReply = ":" + server + " 332 " + nickname + ' ' + info.channelName + " :" + info.channelTopic;
    std::string formatTopicWhoTimeReply = ":" + server + " 333 " + nickname + " " + info.channelName + " " + info.topicSetter + " " + info.lastTopicChangeTime;
    ;

    broadcastMessage(client, formatTopicReply);
    broadcastMessage(client, formatTopicWhoTimeReply);
}

void Channel::updateInviteOnlyMode(Client *client, int flag)
{
    if (flag != 1 && flag != -1)
        return;

    info.inviteOnly = flag;

    const std::string &nickname = client->getCl_str_info(0);
    const std::string modeChange = (flag == 1) ? "+i" : "-i";
    const std::string reply = ":" + nickname + " MODE " + info.channelName + " " + modeChange;

    client->do_TMess(reply, 2);
    broadcastMessage(client, reply);
}

void Channel::updateTopicRestrictionMode(Client *client, int flag)
{
    if (flag != 1 && flag != -1)
        return;

    info.inviteOnly = flag;

    const std::string &nickname = client->getCl_str_info(0);
    const std::string modeChange = (flag == 1) ? "+t" : "-t";
    const std::string reply = ":" + nickname + " MODE " + info.channelName + " " + modeChange;

    client->do_TMess(reply, 2);
    broadcastMessage(client, reply);
}

void Channel::updateChannelKeyMode(Client *client, const std::string &rawArgs, int argIndex, int flag)
{
    const std::string &nickname = client->getCl_str_info(0);
    const std::string modePrefix = ":" + nickname + " MODE " + info.channelName + " ";

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
                std::string sendMessage = ":" + client->getCl_str_info(4) + " 525 " + nickname + ' ' + info.channelName + " :Channel key cannot be 'x'";
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
        if (isUserInChannel(target) == 0)
        {
            std::string errorMessage = "441 " + client->getCl_str_info(0) + ' ' + target + ' ' + info.channelName + ":The user is not in this channel";
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

void Channel::updateUserLimit(Client *client, const std::string &rawArgs, int argIndex, int flag)
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
    const std::string &nickname = client->getCl_str_info(1);

    for (std::list<Client *>::iterator it = operatorsInChannel.begin(); it != operatorsInChannel.end(); ++it)
    {
        std::string message = ":" + server + " 352 " + nickname + ' ' + info.channelName + ' ' +
                              (*it)->getCl_str_info(0) + ' ' + (*it)->getCl_str_info(2) + ' ' +
                              (*it)->getCl_str_info(4) + ' ' + (*it)->getCl_str_info(1) + " @ :0 " + (*it)->getCl_str_info(3);
        client->do_TMess(message, 2);
    }

    for (std::list<Client *>::iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it)
    {
        std::string message = ":" + server + " 352 " + nickname + ' ' + info.channelName + ' ' +
                              (*it)->getCl_str_info(0) + ' ' + (*it)->getCl_str_info(2) + ' ' +
                              (*it)->getCl_str_info(4) + ' ' + (*it)->getCl_str_info(1) + " H :0 " + (*it)->getCl_str_info(3);
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
                updateUserLimit(client, modeArgs, argIndex, flag);
                argIndex++;
                break;
            default:

                client->do_TMess(("472 " + client->getCl_str_info(1) + ' ' + std::string(1, modeChar)), 2);
            }
        }
    }
}

int Channel::validateModeArguments(const std::string &modes, std::string modeArgs)
{
    int givenArgs = 0;
    int expectedArgs = 0;
    int flag = 0;
    std::string currentArg;
    size_t pos;

    // Count actual arguments provided
    while (!modeArgs.empty())
    {
        pos = modeArgs.find(' ');
        currentArg = modeArgs.substr(0, pos);
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

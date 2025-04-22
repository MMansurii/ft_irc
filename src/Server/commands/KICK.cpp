// Server/commands/KICK.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>
+
// Handle the IRC KICK command (channel operator only)
void Server::handleKICK(Client* user, std::istringstream& iss, const std::string& line) {
    std::string channelName, target;
    iss >> channelName >> target;
    std::string comment;
    size_t pos = line.find(" :");
    if (pos != std::string::npos)
        comment = line.substr(pos + 2);
    if (channelName.empty() || target.empty()) {
        user->sendReply("461 KICK :Not enough parameters");
        return;
    }
    Channel* chan = nullptr;
    for (size_t i = 0; i < listOfChannels.size(); ++i) {
        if (listOfChannels[i].first == channelName) {
            chan = listOfChannels[i].second;
            break;
        }
    }
    if (!chan) {
        user->sendReply("403 " + user->getNickname() + " " + channelName + " :No such channel");
        return;
    }
    if (!chan->isUserInChannel(user->getNickname())) {
        user->sendReply("442 " + user->getNickname() + " " + channelName + " :You're not on that channel");
        return;
    }
    if (!chan->isOperatorInChannel(user->getNickname())) {
        user->sendReply("482 " + user->getNickname() + " " + channelName + " :You're not channel operator");
        return;
    }
    if (!chan->isUserInChannel(target)) {
        user->sendReply("441 " + user->getNickname() + " " + target + " " + channelName + " :They aren't on that channel");
        return;
    }
    if (comment.empty())
        comment = user->getNickname();
    chan->handleKickCommand(user, target, comment);
}
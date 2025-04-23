// Server/commands/KICK.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC KICK command
void Server::handleKICK(Client* user, std::istringstream& iss, const std::string& line) {
    std::string chanName;
    iss >> chanName;
    std::string targetNick;
    iss >> targetNick;
    std::string comment;
    size_t pos = line.find(" :");
    if (pos != std::string::npos)
        comment = line.substr(pos + 2);
    if (chanName.empty() || targetNick.empty()) {
        user->sendReply("461 KICK :Not enough parameters");
        return;
    }
    Channel* chan = nullptr;
    for (size_t i = 0; i < listOfChannels.size(); ++i) {
        if (listOfChannels[i].first == chanName) {
            chan = listOfChannels[i].second;
            break;
        }
    }
    if (!chan) {
        user->sendReply("403 " + user->getNickname() + " " + chanName + " :No such channel");
        return;
    }
    if (chan->isOperatorInChannel(user->getNickname()) == 0) {
        user->sendReply("482 " + user->getNickname() + " " + chanName + " :You're not channel operator");
        return;
    }
    if (chan->isUserInChannel(targetNick) == 0) {
        user->sendReply("441 " + user->getNickname() + " " + targetNick + " " + chanName + " :They aren't on that channel");
        return;
    }
    chan->handleKickCommand(user, targetNick, comment);
}
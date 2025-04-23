// Server/commands/KICK.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC KICK command (channel operator only)
void Server::handleKICK(Client* client, std::istringstream& iss, const std::string& line) {
    std::string channelName, target;
    iss >> channelName >> target;
    std::string comment;
    size_t pos = line.find(" :");
    if (pos != std::string::npos)
        comment = line.substr(pos + 2);
    if (channelName.empty() || target.empty()) {
        client->do_TMess("461 KICK :Not enough parameters", 2);
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
        client->do_TMess("403 " + client->getCl_str_info(1) + " " + channelName + " :No such channel", 2);
        return;
    }
    if (!chan->isClientInChannel(client->getCl_str_info(1))) {
        client->do_TMess("442 " + client->getCl_str_info(1) + " " + channelName + " :You're not on that channel", 2);
        return;
    }
    if (!chan->isOperatorInChannel(client->getCl_str_info(1))) {
        client->do_TMess("482 " + client->getCl_str_info(1) + " " + channelName + " :You're not channel operator", 2);
        return;
    }
    if (!chan->isClientInChannel(target)) {
        client->do_TMess("441 " + client->getCl_str_info(1) + " " + target + " " + channelName + " :They aren't on that channel", 2);
        return;
    }
    if (comment.empty())
        comment = client->getCl_str_info(1);
    chan->handleKickCommand(client, target, comment);
}
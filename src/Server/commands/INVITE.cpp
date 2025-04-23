// Server/commands/INVITE.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC INVITE command (channel operator only)
void Server::handleINVITE(Client* client, std::istringstream& iss, const std::string& /*line*/) {
    std::string targetNick, channelName;
    iss >> targetNick >> channelName;
    if (targetNick.empty() || channelName.empty()) {
        client->do_TMess("461 INVITE :Not enough parameters", 2);
        return;
    }
    Channel* chan = nullptr;
    for (auto &p : listOfChannels) {
        if (p.first == channelName) { chan = p.second; break; }
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
    Client* targetClient = nullptr;
    for (auto &u : listOfClients) {
        if (u.second->getCl_str_info(1) == targetNick) {
            targetClient = u.second;
            break;
        }
    }
    if (!targetClient) {
        client->do_TMess("401 " + client->getCl_str_info(1) + " " + targetNick + " :No such nick/channel", 2);
        return;
    }
    chan->addInvitedGuest(targetClient);
    // Notify target of invitation
    std::string inviteMsg = ":" + client->getCl_str_info(1) + "!" + client->getCl_str_info(0) +
                             "@" + client->getCl_str_info(2) +
                             " INVITE " + targetNick + " :" + channelName;
    targetClient->do_TMess(inviteMsg, 2);
    // Confirm to inviter
    std::string srv = client->getCl_str_info(4);
    client->do_TMess(":" + srv + " 341 " + client->getCl_str_info(1) + " " + targetNick + " " + channelName, 2);
}
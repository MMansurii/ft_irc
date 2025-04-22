// Server/commands/INVITE.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC INVITE command (channel operator only)
void Server::handleINVITE(Client* user, std::istringstream& iss, const std::string& /*line*/) {
    std::string targetNick, channelName;
    iss >> targetNick >> channelName;
    if (targetNick.empty() || channelName.empty()) {
        user->sendReply("461 INVITE :Not enough parameters");
        return;
    }
    Channel* chan = nullptr;
    for (auto &p : listOfChannels) {
        if (p.first == channelName) { chan = p.second; break; }
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
    Client* targetClient = nullptr;
    for (auto &u : listOfUsers) {
        if (u.second->getNickname() == targetNick) {
            targetClient = u.second;
            break;
        }
    }
    if (!targetClient) {
        user->sendReply("401 " + user->getNickname() + " " + targetNick + " :No such nick/channel");
        return;
    }
    chan->addInvitedGuest(targetClient);
    // Notify target of invitation
    std::string inviteMsg = ":" + user->getNickname() + "!" + user->getCl_str_info(0) +
                             "@" + user->getCl_str_info(2) +
                             " INVITE " + targetNick + " :" + channelName;
    targetClient->sendReply(inviteMsg);
    // Confirm to inviter
    std::string srv = user->getCl_str_info(4);
    user->sendReply(":" + srv + " 341 " + user->getNickname() + " " + targetNick + " " + channelName);
}
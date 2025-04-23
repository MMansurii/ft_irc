// Server/commands/INVITE.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC INVITE command
void Server::handleINVITE(Client* user, std::istringstream& iss, const std::string& /*line*/) {
    std::string targetNick;
    iss >> targetNick;
    std::string chanName;
    iss >> chanName;
    if (targetNick.empty() || chanName.empty()) {
        user->sendReply("461 INVITE :Not enough parameters");
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
    Client* targetUser = nullptr;
    for (size_t i = 0; i < listOfUsers.size(); ++i) {
        if (listOfUsers[i].second->getNickname() == targetNick) {
            targetUser = listOfUsers[i].second;
            break;
        }
    }
    if (!targetUser) {
        user->sendReply("401 " + user->getNickname() + " " + targetNick + " :No such nick");
        return;
    }
    chan->addInvitedGuest(targetUser);
    // Notify the target user of the invitation
    std::string inviteMsg = ":" + user->getCl_str_info(1) + "!" + user->getCl_str_info(0) + "@" + user->getCl_str_info(2)
        + " INVITE " + targetNick + " " + chanName;
    targetUser->sendReply(inviteMsg);
    // Confirm to inviter with RPL_INVITING (341)
    std::string rplInvite = ":" + user->getCl_str_info(4) + " 341 " + user->getCl_str_info(1)
        + " " + targetNick + " " + chanName;
    user->sendReply(rplInvite);
}
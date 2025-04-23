// Server/commands/MODE.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC MODE command
void Server::handleMODE(Client* user, std::istringstream& iss, const std::string& line) {
    (void)line; // avoid unused parameter warning when modes are used
    std::string target;
    iss >> target;
    if (target.empty()) {
        user->sendReply("461 MODE :Not enough parameters");
        return;
    }
    // Channel mode
    if (!target.empty() && target[0] == '#') {
        std::string modes;
        iss >> modes;
        // Extract mode arguments (everything after modes token)
        std::string rest;
        std::getline(iss, rest);
        if (!rest.empty() && rest[0] == ' ')
            rest.erase(0, 1);
        Channel* chan = nullptr;
        for (size_t i = 0; i < listOfChannels.size(); ++i) {
            if (listOfChannels[i].first == target) {
                chan = listOfChannels[i].second;
                break;
            }
        }
        if (!chan) {
            user->sendReply("403 " + user->getNickname() + " " + target + " :No such channel");
            return;
        }
        if (chan->isOperatorInChannel(user->getNickname()) == 0) {
            user->sendReply("482 " + user->getNickname() + " " + target + " :You're not channel operator");
            return;
        }
        if (modes.empty()) {
            std::string modeList = chan->getChannelModes();
            if (modeList.empty())
                user->sendReply("324 " + user->getNickname() + " " + target + " :No modes are set");
            else
                user->sendReply("324 " + user->getNickname() + " " + target + " " + modeList);
            return;
        }
        chan->applyChannelModes(user, modes, rest);
    } else {
        // User mode is not implemented
        user->sendReply("501 :Unknown MODE flag");
    }
}
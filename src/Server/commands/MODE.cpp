// Server/commands/MODE.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC MODE command
void Server::handleMODE(Client* client, std::istringstream& iss, const std::string& line) {
    (void)line; // avoid unused parameter warning when modes are used
    std::string target;
    iss >> target;
    if (target.empty()) {
        client->do_TMess("461 MODE :Not enough parameters", 2);
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
            client->do_TMess("403 " + client->getCl_str_info(1) + " " + target + " :No such channel", 2);
            return;
        }
        if (chan->isOperatorInChannel(client->getCl_str_info(1)) == 0) {
            client->do_TMess("482 " + client->getCl_str_info(1) + " " + target + " :You're not channel operator", 2);
            return;
        }
        if (modes.empty()) {
            std::string modeList = chan->getChannelModes();
            if (modeList.empty())
                client->do_TMess("324 " + client->getCl_str_info(1) + " " + target + " :No modes are set", 2);
            else
                client->do_TMess("324 " + client->getCl_str_info(1) + " " + target + " " + modeList, 2);
            return;
        }
        chan->applyChannelModes(client, modes, rest);
    } else {
        // Client mode is not implemented
        client->do_TMess("501 :Unknown MODE flag", 2);
    }
}
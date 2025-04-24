// Server/commands/PRIVMSG.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC PRIVMSG command
void Server::handlePRIVMSG(Client* client, std::istringstream& iss, const std::string& line) {
  std::string target;
  iss >> target;
  size_t posm = line.find(" :");
  std::string content =
    (posm != std::string::npos ? line.substr(posm + 2) : "");
  if (target.empty() || content.empty()) {
    client->do_TMess("461 PRIVMSG :Not enough parameters", 2);
    return;
  }
  std::string full = ":" + client->getCl_str_info(1) + " PRIVMSG " + target + " :" + content;
  if (!target.empty() && target[0] == '#') {
    // Channel message: ensure client is a channel member
    for (size_t j = 0; j < listOfChannels.size(); ++j) {
      if (listOfChannels[j].first == target) {
        Channel* chan = listOfChannels[j].second;
        // Check membership (either regular client or operator)
        if (chan->isClientInChannel(client->getCl_str_info(1)) == 0
            && chan->isOperatorInChannel(client->getCl_str_info(1)) == 0) {
          client->do_TMess("442 " + client->getCl_str_info(1) + " " + target + " :You're not on that channel", 2);
        } else {
          chan->broadcastMessage(client, full);
        }
        break;
      }
    }
  } else {
    for (size_t j = 0; j < listOfClients.size(); ++j) {
      Client* u = listOfClients[j].second;
      if (u->getCl_str_info(1) == target) {
        u->do_TMess(full, 2);
        break;
      }
    }
  }
}
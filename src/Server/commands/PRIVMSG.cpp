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
    for (size_t j = 0; j < listOfChannels.size(); ++j) {
      if (listOfChannels[j].first == target) {
        listOfChannels[j].second->broadcastMessage(client, full);
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
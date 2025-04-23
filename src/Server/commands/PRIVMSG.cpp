// Server/commands/PRIVMSG.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC PRIVMSG command
void Server::handlePRIVMSG(Client* user, std::istringstream& iss, const std::string& line) {
  std::string target;
  iss >> target;
  size_t posm = line.find(" :");
  std::string content =
    (posm != std::string::npos ? line.substr(posm + 2) : "");
  if (target.empty() || content.empty()) {
    user->sendReply("461 PRIVMSG :Not enough parameters");
    return;
  }
  std::string full = ":" + user->getNickname() + " PRIVMSG " + target + " :" + content;
  if (!target.empty() && target[0] == '#') {
    for (size_t j = 0; j < listOfChannels.size(); ++j) {
      if (listOfChannels[j].first == target) {
        listOfChannels[j].second->broadcastMessage(user, full);
        break;
      }
    }
  } else {
    for (size_t j = 0; j < listOfUsers.size(); ++j) {
      Client* u = listOfUsers[j].second;
      if (u->getNickname() == target) {
        u->sendReply(full);
        break;
      }
    }
  }
}
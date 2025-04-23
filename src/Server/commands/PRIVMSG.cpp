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
    Channel* chan = nullptr;
    for (size_t j = 0; j < listOfChannels.size(); ++j) {
      if (listOfChannels[j].first == target) {
        chan = listOfChannels[j].second;
        break;
      }
    }
    if (!chan) {
      user->sendReply("403 " + user->getNickname() + " " + target + " :No such channel");
      return;
    }
    // Check channel membership
    if (chan->isUserInChannel(user->getNickname()) == 0 &&
        chan->isOperatorInChannel(user->getNickname()) == 0) {
      user->sendReply("404 " + user->getNickname() + " " + target + " :Cannot send to channel");
      return;
    }
    chan->broadcastMessage(user, full);
  } else {
    bool found = false;
    for (size_t j = 0; j < listOfUsers.size(); ++j) {
      Client* u = listOfUsers[j].second;
      if (u->getNickname() == target) {
        u->sendReply(full);
        found = true;
        break;
      }
    }
    if (!found) {
      user->sendReply("401 " + user->getNickname() + " " + target + " :No such nick");
    }
  }
}
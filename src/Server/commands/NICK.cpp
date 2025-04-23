// Server/commands/NICK.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC NICK command
void Server::handleNICK(Client* user, std::istringstream& iss, const std::string& /*line*/) {
  std::string nick;
  iss >> nick;
  if (nick.empty()) {
    user->sendReply("431 :No nickname given");
    return;
  }
  for (size_t i = 0; i < listOfUsers.size(); ++i) {
    Client* u = listOfUsers[i].second;
    if (u != user && u->getNickname() == nick) {
      user->sendReply("433 * " + nick + " :Nickname is already in use");
      return;
    }
  }
  user->setCl_str_info(1, nick);
}
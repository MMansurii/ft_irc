// Server/commands/NICK.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC NICK command
void Server::handleNICK(Client* client, std::istringstream& iss, const std::string& /*line*/) {
  std::string nick;
  iss >> nick;
  if (nick.empty()) {
    client->do_TMess("431 :No nickname given", 2);
    return;
  }
  for (size_t i = 0; i < listOfClients.size(); ++i) {
    Client* u = listOfClients[i].second;
    if (u != client && u->getCl_str_info(1) == nick) {
      client->do_TMess("433 * " + nick + " :Nickname is already in use", 2);
      return;
    }
  }
  client->setCl_str_info(1, nick);
}
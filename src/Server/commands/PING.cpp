// Server/commands/PING.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC PING command
void Server::handlePING(Client* client, std::istringstream& iss, const std::string& /*line*/) {
  std::string token;
  iss >> token;
  client->do_TMess("PONG " + token, 2);
}
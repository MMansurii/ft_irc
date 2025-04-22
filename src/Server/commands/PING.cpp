// Server/commands/PING.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC PING command
void Server::handlePING(Client* user, std::istringstream& iss, const std::string& /*line*/) {
  std::string token;
  iss >> token;
  user->sendReply("PONG " + token);
}
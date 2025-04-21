// Server/commands/PASS.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC PASS command
void Server::handlePASS(Client* user, std::istringstream& iss, const std::string& /*line*/) {
  std::string pass;
  iss >> pass;
  if (user->getCl_int_info(0)) {
    user->sendReply("462 You may not reregister");
  } else if (pass != passwordValue) {
    user->sendReply("464 :Password incorrect");
    disconnectUnauthenticatedClient(user);
  } else {
    user->setCl_int_info(0, 1);
  }
}
// Server/commands/PASS.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC PASS command
void Server::handlePASS(Client* client, std::istringstream& iss, const std::string& /*line*/) {
  std::string pass;
  iss >> pass;
  if (client->getCl_int_info(0)) {
    client->do_TMess("462 You may not reregister", 2);
  } else if (pass != passwordValue) {
    client->do_TMess("464 :Password incorrect", 2);
    disconnectUnauthenticatedClient(client);
  } else {
    client->setCl_int_info(0, 1);
  }
}
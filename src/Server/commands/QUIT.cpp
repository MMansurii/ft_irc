#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC QUIT command
void Server::handleQUIT(Client *client, std::istringstream & /*iss*/,
                        const std::string &line) {
  std::string comment;
  size_t p = line.find(' ');
  if (p != std::string::npos)
    comment = line.substr(p + 1);
  client->do_TMess(":" + client->getCl_str_info(1) + " QUIT " + comment, 2);
  int fd = client->getCl_int_info(2);
  closeSocketAndErasePollfd(fd);
  eraseClientFromClientList(fd);
}

#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC QUIT command
void Server::handleQUIT(Client *user, std::istringstream & /*iss*/,
                        const std::string &line) {
  std::string comment;
  size_t p = line.find(' ');
  if (p != std::string::npos)
    comment = line.substr(p + 1);
  user->sendReply(":" + user->getNickname() + " QUIT " + comment);
  int fd = user->getUserFd();
  closeSocketAndErasePollfd(fd);
  eraseUserFromUserList(fd);
}

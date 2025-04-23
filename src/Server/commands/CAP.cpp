// Server/commands/CAP.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC CAP command
void Server::handleCAP(Client *user, std::istringstream &iss,
                       const std::string & /*line*/) {
  std::string sub;
  iss >> sub;
  std::string srv = user->getCl_str_info(4);
  if (sub == "LS") {
    user->sendReply(std::string(":") + srv + " CAP * LS :multi-prefix");
  } else if (sub == "REQ") {
    std::string caps;
    std::getline(iss, caps);
    if (!caps.empty() && caps[0] == ' ')
      caps.erase(0, 1);
    if (!caps.empty() && caps[0] == ':')
      caps.erase(0, 1);
    if (caps == "multi-prefix") {
      user->sendReply(std::string(":") + srv + " CAP * ACK :" + caps);
    } else {
      user->sendReply(std::string(":") + srv + " CAP * NAK :" + caps);
    }
  }
}

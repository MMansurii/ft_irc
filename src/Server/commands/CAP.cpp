#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC CAP command
void Server::handleCAP(Client *client, std::istringstream &iss,
                       const std::string & /*line*/) {
  std::string sub;
  iss >> sub;
  std::string srv = client->getCl_str_info(4);
  if (sub == "LS") {
    client->do_TMess(std::string(":") + srv + " CAP * LS :multi-prefix", 2);
  } else if (sub == "REQ") {
    std::string caps;
    std::getline(iss, caps);
    if (!caps.empty() && caps[0] == ' ')
      caps.erase(0, 1);
    if (!caps.empty() && caps[0] == ':')
      caps.erase(0, 1);
    if (caps == "multi-prefix") {
      client->do_TMess(std::string(":") + srv + " CAP * ACK :" + caps, 2);
    } else {
      client->do_TMess(std::string(":") + srv + " CAP * NAK :" + caps, 2);
    }
  }
}

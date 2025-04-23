// Server/commands/PART.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>
#include <vector>

// Handle the IRC PART command
void Server::handlePART(Client* user, std::istringstream& iss, const std::string& line) {
  std::string chlist;
  iss >> chlist;
  std::string comment;
  size_t posc = line.find(" :");
  if (posc != std::string::npos)
    comment = line.substr(posc + 2);
  if (chlist.empty()) {
    user->sendReply("461 PART :Not enough parameters");
    return;
  }
  std::istringstream pcs(chlist);
  std::string cname;
  while (std::getline(pcs, cname, ',')) {
    for (size_t j = 0; j < listOfChannels.size(); ++j) {
      if (listOfChannels[j].first == cname) {
        Channel* chan = listOfChannels[j].second;
        std::string partMsg = 
          ":" + user->getNickname() + "!" + user->getNickname() +
          "@" + user->getCl_str_info(2) + " PART " + cname +
          (comment.empty() ? "" : " :" + comment);
        chan->broadcastMessage(user, partMsg);
        user->sendReply(partMsg);
        chan->removeClientsInChannel(user->getNickname());
        if (chan->getChannelDetail(CURRENT_CLIENT_COUNT) == "0") {
          delete chan;
          listOfChannels.erase(listOfChannels.begin() + j);
        }
        break;
      }
    }
  }
}
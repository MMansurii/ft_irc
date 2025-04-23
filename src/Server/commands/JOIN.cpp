// Server/commands/JOIN.cpp
#include "../Server.hpp"
#include <sstream>
#include <vector>
#include <string>

// Handle the IRC JOIN command
void Server::handleJOIN(Client* user, std::istringstream& iss, const std::string& /*line*/) {
  std::string chlist;
  iss >> chlist;
  std::string key;
  iss >> key;
  if (chlist.empty()) {
    user->sendReply("461 JOIN :Not enough parameters");
    return;
  }
  std::vector<std::string> chans;
  std::istringstream cs(chlist);
  std::string cname;
  while (std::getline(cs, cname, ','))
    chans.push_back(cname);
  std::vector<std::string> keys;
  if (!key.empty()) {
    std::istringstream ks(key);
    std::string kk;
    while (std::getline(ks, kk, ','))
      keys.push_back(kk);
  }
  for (size_t i = 0; i < chans.size(); ++i) {
    const std::string& chanName = chans[i];
    const std::string& chanKey = (i < keys.size() ? keys[i] : "");
    Channel* chan = nullptr;
    for (size_t j = 0; j < listOfChannels.size(); ++j) {
      if (listOfChannels[j].first == chanName) {
        chan = listOfChannels[j].second;
        break;
      }
    }
    if (!chan) {
      chan = new Channel(chanName, chanKey, user);
      listOfChannels.push_back(std::make_pair(chanName, chan));
      std::string joinMsg = 
        ":" + user->getNickname() + "!" + user->getNickname() +
        "@" + user->getCl_str_info(2) + " JOIN " + chanName;
      user->sendReply(joinMsg);
      chan->sendUserListToClient(user);
    } else {
      std::string res = chan->attemptJoinChannel(user, chanKey);
      if (!res.empty() && res[0] == ':') {
        user->sendReply(res);
        chan->broadcastMessage(user, res);
        chan->sendUserListToClient(user);
      } else {
        user->sendReply(res);
      }
    }
  }
}
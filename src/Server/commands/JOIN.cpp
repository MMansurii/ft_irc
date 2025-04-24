// Server/commands/JOIN.cpp
#include "../Server.hpp"
#include <sstream>
#include <vector>
#include <string>

// Handle the IRC JOIN command
void Server::handleJOIN(Client* client, std::istringstream& iss, const std::string& /*line*/) {
  std::string chlist;
  iss >> chlist;
  std::string key;
  iss >> key;
  if (chlist.empty()) {
    client->do_TMess("461 JOIN :Not enough parameters", 2);
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
      chan = new Channel(chanName, chanKey, client);
      listOfChannels.push_back(std::make_pair(chanName, chan));
      std::string joinMsg = 
        ":" + client->getCl_str_info(1) + "!" + client->getCl_str_info(1) +
        "@" + client->getCl_str_info(2) + " JOIN " + chanName;
      client->do_TMess(joinMsg, 2);
      // After joining, send topic (or no-topic) and then the NAMES list
      chan->sendTopicToClient(client);
      chan->sendClientListToClient(client);
    } else {
      std::string res = chan->attemptJoinChannel(client, chanKey);
      if (!res.empty() && res[0] == ':') {
        client->do_TMess(res, 2);
        chan->broadcastMessage(client, res);
        // After joining, send topic (or no-topic) and then the NAMES list
        chan->sendTopicToClient(client);
        chan->sendClientListToClient(client);
      } else {
        client->do_TMess(res, 2);
      }
    }
  }
}
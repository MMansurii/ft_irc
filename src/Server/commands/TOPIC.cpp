// Server/commands/TOPIC.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>
#include <ctime>

// Handle the IRC TOPIC command
void Server::handleTOPIC(Client* client, std::istringstream& iss, const std::string& line) {
    std::string chanName;
    iss >> chanName;
    if (chanName.empty()) {
        client->do_TMess("461 TOPIC :Not enough parameters", 2);
        return;
    }
    // Find channel
    Channel* chan = nullptr;
    for (size_t i = 0; i < listOfChannels.size(); ++i) {
        if (listOfChannels[i].first == chanName) {
            chan = listOfChannels[i].second;
            break;
        }
    }
    if (!chan) {
        client->do_TMess("403 " + client->getCl_str_info(1) + " " + chanName + " :No such channel", 2);
        return;
    }
    // Extract new topic if provided
    std::string newTopic;
    size_t pos = line.find(" :");
    if (pos != std::string::npos)
        newTopic = line.substr(pos + 2);
    if (newTopic.empty()) {
        // Query topic
        std::string topic = chan->getChannelDetail(CHANNEL_TOPIC);
        if (topic.empty()) {
            client->do_TMess("331 " + client->getCl_str_info(1) + " " + chanName + " :No topic is set", 2);
        } else {
            client->do_TMess("332 " + client->getCl_str_info(1) + " " + chanName + " :" + topic, 2);
            std::string setter = chan->getChannelDetail(TOPIC_SETTER);
            std::string ttime = chan->getChannelDetail(TOPIC_TIME);
            client->do_TMess("333 " + client->getCl_str_info(1) + " " + chanName + " " + setter + " " + ttime, 2);
        }
    } else {
        // Set topic
        // Check permission if topic restricted
        if (chan->getChannelDetail(TOPIC_MODE) == "1" && chan->isOperatorInChannel(client->getCl_str_info(1)) == 0) {
            client->do_TMess("482 " + client->getCl_str_info(1) + " " + chanName + " :You're not channel operator", 2);
            return;
        }
        // Broadcast topic update
        chan->broadcastTopicUpdate(client, newTopic);
    }
}
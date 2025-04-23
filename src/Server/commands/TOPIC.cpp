// Server/commands/TOPIC.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>
#include <ctime>

// Handle the IRC TOPIC command
void Server::handleTOPIC(Client* user, std::istringstream& iss, const std::string& line) {
    std::string chanName;
    iss >> chanName;
    if (chanName.empty()) {
        user->sendReply("461 TOPIC :Not enough parameters");
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
        user->sendReply("403 " + user->getNickname() + " " + chanName + " :No such channel");
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
            user->sendReply("331 " + user->getNickname() + " " + chanName + " :No topic is set");
        } else {
            user->sendReply("332 " + user->getNickname() + " " + chanName + " :" + topic);
            std::string setter = chan->getChannelDetail(TOPIC_SETTER);
            std::string ttime = chan->getChannelDetail(TOPIC_TIME);
            user->sendReply("333 " + user->getNickname() + " " + chanName + " " + setter + " " + ttime);
        }
    } else {
        // Set topic
        // Check permission if topic restricted
        if (chan->getChannelDetail(TOPIC_MODE) == "1" && chan->isOperatorInChannel(user->getNickname()) == 0) {
            user->sendReply("482 " + user->getNickname() + " " + chanName + " :You're not channel operator");
            return;
        }
        // Broadcast topic update
        chan->broadcastTopicUpdate(user, newTopic);
    }
}
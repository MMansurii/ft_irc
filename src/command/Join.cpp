#include "Command_monitor.hpp"

void handle_join_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state) {
    if (!client_state.registered) {
        std::string err = ":irc.example.com 451 * :You have not registered\r\n";
        send(client_socket, err.c_str(), err.length(), 0);
        return;
    }
    
    if (params.size() >= 1) {
        std::string channels = params[0];
        std::istringstream channel_stream(channels);
        std::string channel;
        
        while (std::getline(channel_stream, channel, ',')) {
            if (channel.empty() || channel[0] != '#') {
                continue;
            }
            client_state.channels.push_back(channel);
            std::string join_msg = ":" + client_state.nickname + "!user@localhost JOIN " + channel + "\r\n";
            send(client_socket, join_msg.c_str(), join_msg.length(), 0);
            
            std::string topic_msg = ":irc.example.com 331 " + client_state.nickname + " " + channel + " :No topic is set\r\n";
            send(client_socket, topic_msg.c_str(), topic_msg.length(), 0);
            
            std::string names_prefix = ":irc.example.com 353 " + client_state.nickname + " = " + channel + " :@" + client_state.nickname + "\r\n";
            send(client_socket, names_prefix.c_str(), names_prefix.length(), 0);
            
            std::string names_end = ":irc.example.com 366 " + client_state.nickname + " " + channel + " :End of /NAMES list.\r\n";
            send(client_socket, names_end.c_str(), names_end.length(), 0);
        }
    }

}

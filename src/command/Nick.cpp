#include "Command_monitor.hpp"
#include <sys/socket.h>
#include <unistd.h>

void handle_nick_command(int client_socket, const std::string& nick, ClientState& client_state) {
    client_state.nickname = nick;
    std::string nick_reply = ":" + (client_state.nickname.empty() ? "*" : client_state.nickname) +
                             "!user@localhost NICK :" + nick + "\r\n";
    send(client_socket, nick_reply.c_str(), nick_reply.length(), 0);
}

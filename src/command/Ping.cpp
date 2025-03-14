#include "Command_monitor.hpp"
#include <sys/socket.h>
#include <unistd.h>

void handle_ping_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state) 
{
    std::string pong = "PONG :" + (params.empty() ? "irc.example.com" : params[0]) + "\r\n";
    send(client_socket, pong.c_str(), pong.length(), 0);
}

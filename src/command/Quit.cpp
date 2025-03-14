#include "Command_monitor.hpp"
#include <unistd.h>
#include <iostream>

void handle_quit_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state) 
{
    std::cout << "Client requested to quit." << std::endl;
    close(client_socket);
}

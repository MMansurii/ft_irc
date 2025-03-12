#include "Command_monitor.hpp"
#include <unistd.h>
#include <iostream>

void handle_quit_command(int client_socket) {
    std::cout << "Client requested to quit." << std::endl;
    close(client_socket);
}

#include "src/utils4main.hpp"
#include "src/Server/Server.hpp"
#include <iostream>
#include <string>
#include <signal.h>

Server* server = nullptr;

void check_Signal(int sig) 
{
    if (sig == SIGINT && server != nullptr) 
	{
        std::cout << "\nReceived SIGINT. Shutting down server...\n";
        server->stop();
        exit(0);
    }
}


int main(int argc, char **argv) 
{

    if (argc != 3)
	{
        showError(1, argc, argv);
        return 1;
    }
    if (!checkport(argv[1]))
        return 1;
    if (!check_password(argv[2]))
        return 1;
    try 
	{
        int port = std::stoi(argv[1]);
        std::string password = argv[2];
        
        signal(SIGINT, check_Signal);
        server = new Server(port, password);
        if (!server->initialize()) 
		{
            std::cerr << "Failed to initialize server.\n";
            delete server;
            return 1;
        }
        server->start();
    }
    catch (std::exception &e) 
	{
        stop_server();
        std::cerr << "\033[31mError: " << e.what() << "\033[0m\n";
        return 1;
    }
    return 0;
}

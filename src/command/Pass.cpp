#include "Command_monitor.hpp"
#include "../Server/Server.hpp"

void handle_pass_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state)
{
    if (params.empty()) 
	{
        std::string error = "461 PASS :Not enough parameters\r\n";
        send(client_socket, error.c_str(), error.length(), 0);
        return;
    }
    if (client_state.isRegistered()) 
	{
        std::string error = "462 :You may not reregister\r\n";
        send(client_socket, error.c_str(), error.length(), 0);
        return;
    }
    const std::string& server_password = Server::getPassword();
    if (params[0] == server_password) 
        client_state.passwordProvided = true;
	else 
        client_state.passwordProvided = false;
}

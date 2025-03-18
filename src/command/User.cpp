#include "Command_monitor.hpp"


void handle_user_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state)
{
    if (params.size() < 4)
	{
        std::string error = "461 USER :Not enough parameters\r\n";
        send(client_socket, error.c_str(), error.length(), 0);
        return;
    }
    if (client_state.isRegistered())
	{
        std::string error = "462 :You may not reregister\r\n";
        send(client_socket, error.c_str(), error.length(), 0);
        return;
    }
    client_state.username = params[0];
    client_state.realname = params[3];
}

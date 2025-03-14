#include "Command_monitor.hpp"
#include <sys/socket.h>
#include <unistd.h>

void handle_cap_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state) 
{
    if (params.size() >= 1)
	{
        if (params[0] == "LS")
		{
            std::string response = ":irc.example.com CAP * LS :\r\n";
            send(client_socket, response.c_str(), response.length(), 0);
        }
        else if (params[0] == "REQ")
		{
            if (params.size() >= 2)
			{
                std::string cap_ack = ":irc.example.com CAP * ACK :" + params[1] + "\r\n";
                send(client_socket, cap_ack.c_str(), cap_ack.length(), 0);
            }
        } 
		else if (params[0] == "LIST")
		{
            std::string response = ":irc.example.com CAP * LIST :\r\n";
            send(client_socket, response.c_str(), response.length(), 0);
        }
    }
}

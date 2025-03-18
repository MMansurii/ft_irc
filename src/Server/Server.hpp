#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../command/ClientState.hpp"

class Server 
{
	private:
		int server_fd;
		int port;
		std::string password;
		static std::string instance_password;
		struct sockaddr_in server_addr;
		bool running;
		void send_welcome_messages(int client_socket, const std::string& nickname);
		void handle_client(int client_socket);

	public:
		Server(int port, const std::string& password);
		~Server();
		bool initialize();
		void start();
		void stop();
		int getPort() const { return port; }
		static std::string getPassword() { return instance_password; }
		bool isRunning() const { return running; }
		static const std::string& getStaticPassword() { return instance_password; }
};
#endif

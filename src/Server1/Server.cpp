
#include "Server.hpp"

Server::Server(std::string port) : _port(port), _listeningSocket(-1) {}

void Server::initializeServer() {
	struct addrinfo hints, *resolvedInfo = NULL;
	_initializeAddressHints(hints);
	_resolveBindingAddress(hints, resolvedInfo);

	int socketFD = _createSocketFromAddress(resolvedInfo);
	_bindSocketToResolvedAddress(socketFD, resolvedInfo);
	_enableSocketListening(socketFD);

	_listeningSocket = socketFD;
	freeaddrinfo(resolvedInfo);
}

void Server::_initializeAddressHints(struct addrinfo &hints) {
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE; // Bind to local IP
}

void Server::_resolveBindingAddress(struct addrinfo &hints, struct addrinfo *&resolvedInfo) {
	int status = getaddrinfo(NULL, _port.c_str(), &hints, &resolvedInfo);
	if (status != 0) {
		std::cerr << "\033[31m Error: Could not resolve local address. Reason: " << gai_strerror(status) << " \033[0m\n";
		exit(1);
	}
	std::cout << "\033[32m [OK] \033[0m\033[90m Address info resolved successfully \033[0m\n";
}

int Server::_createSocketFromAddress(struct addrinfo *resolvedInfo) {
	int socketFD = socket(resolvedInfo->ai_family, resolvedInfo->ai_socktype, resolvedInfo->ai_protocol);
	if (socketFD == -1) {
		std::cerr << "\033[31m Error: Failed to create socket \033[0m\n";
		freeaddrinfo(resolvedInfo);
		exit(1);
	}
	std::cout << "\033[32m [OK] \033[0m\033[90m Socket created successfully \033[0m\n";
	return socketFD;
}

void Server::_bindSocketToResolvedAddress(int socketFD, struct addrinfo *resolvedInfo) {
	if (bind(socketFD, resolvedInfo->ai_addr, resolvedInfo->ai_addrlen) == -1) {
		std::cerr << "\033[31m Error: Failed to bind socket \033[0m\n";
		close(socketFD);
		freeaddrinfo(resolvedInfo);
		exit(1);
	}
	std::cout << "\033[32m [OK] \033[0m\033[90m Socket successfully bound to address \033[0m\n";
}

void Server::_enableSocketListening(int socketFD) {
	if (listen(socketFD, 1000) == -1) {
		std::cerr << "\033[31m Error: Failed to listen on socket \033[0m\n";
		close(socketFD);
		exit(1);
	}
	std::cout << "\033[32m [OK] \033[0m\033[90m Server listening on port " << _port << " \033[0m\n";
}

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <poll.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cstdlib>

class Server {
private:
    std::string _port;               // Port to listen on
    int _listeningSocket;            // Main socket file descriptor

    // --- Helper methods for server setup ---
    
    // Configures address hints for getaddrinfo
    void _initializeAddressHints(struct addrinfo &hints);

    // Resolves an address using the configured hints
    void _resolveBindingAddress(struct addrinfo &hints, struct addrinfo *&resolvedInfo);

    // Creates a socket from the resolved address
    int _createSocketFromAddress(struct addrinfo *resolvedInfo);

    // Binds the socket to the local address
    void _bindSocketToResolvedAddress(int socketFD, struct addrinfo *resolvedInfo);

    // Starts listening on the bound socket
    void _enableSocketListening(int socketFD);

public:
    Server(std::string port);
    
    // Initializes the server (creates, binds, and listens)
    void initializeServer();
};

#endif
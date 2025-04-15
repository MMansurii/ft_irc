#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <poll.h>
#include <vector>

// The Server class is responsible for setting up a listening TCP socket
// and preparing the server to accept client connections on a given port.
class Server {
private:
    std::string portValue;            // Port number to bind to (e.g., "6667")
    int listeningSocket;              // File descriptor for the listening socket
    std::string serverCreatedAt;      // Timestamp of server creation

    // Step 1: Fill addrinfo struct with desired socket options (IPv4/6, TCP, etc.)
    void prepareSocketConfiguration(struct addrinfo &config);

    // Step 2: Resolve local address using getaddrinfo()
    void resolveLocalAddress(struct addrinfo &config, struct addrinfo *&addressList);

    // Step 3: Create socket using the resolved address info
    int createSocket(struct addrinfo *addressList);

    // Step 4: Bind socket to resolved IP address and port
    void bindSocketToAddress(int fd, struct addrinfo *addressList);

    // Step 5: Start listening on the socket
    void enableListening(int fd, struct addrinfo *addressList);


    std::string generateCurrentTime(const std::string& timeFormat);  // generates time string


public:
    // Constructor that sets the port value
    Server(std::string port);

    // Public function to start the server initialization process
    void startServer();

    void setupCommandHandlers(); // New: command handler map initialization
};

#endif

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <poll.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <ctime>

class User;  // Forward declaration

class Server {
public:
    // Constructor
    Server(std::string port);

    // Starts server setup and run loop
    void startServer();

    // Main server loop
    void runMainLoop();

    // Set up command routing (to be implemented)
    void setupCommandHandlers();

private:
    // Server properties
    std::string portValue;
    int mainSocketDescriptor;
    std::string serverCreatedAt;
    bool isServerActive = true;

    // Client management
    std::vector<std::pair<int, User*> > listOfUsers;
    std::vector<struct pollfd> listOfPollDescriptors;

    // ===== Socket Setup Functions =====

    // Prepares configuration for getaddrinfo()
    void prepareSocketConfiguration(struct addrinfo& config);

    // Resolves hostname and service into usable address
    void resolveLocalAddress(struct addrinfo& config, struct addrinfo*& addressList);

    // Creates socket from address info
    int createSocket(struct addrinfo* addressList);

    // Binds socket to an address
    void bindSocketToAddress(int fd, struct addrinfo* addressList);

    // Enables listening for incoming connections
    void enableListening(int fd, struct addrinfo* addressList);

    // Returns current system time in string format
    std::string generateCurrentTime(const std::string& timeFormat);

    // ===== Server Runtime Functions =====

    // Prepares pollfd vector with server socket
    void preparePollingList();

    // Calls poll() and dispatches events
    void evaluatePollEvents();

    // Handles poll() failure case
    void handlePollFailure();

    // Checks each file descriptor for readiness
    void processEachPollfd();

    // Determines how to handle a ready fd
    void checkPollfdState(std::vector<struct pollfd>::iterator descriptor);

    // Handles readable or writable state
    void handleIncomingOrOutgoingData(std::vector<struct pollfd>::iterator descriptor);

    // Handles error or hangup state
    void handleConnectionIssue(std::vector<struct pollfd>::iterator descriptor);

    // Closes all connections and cleans up
    void shutdownGracefully();

    // Clears user list, closes sockets
    void cleanupAllResources();

    // Disconnects a single user
    void terminateClientConnection(std::vector<struct pollfd>::iterator descriptor, const std::string& message);

    // Placeholder — should disconnect user from all channels (if implemented)
    void leaveAllUserChannels(User* user);
};

#endif

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <poll.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "User.hpp"
#include "Channel.hpp"

class Server {
public:
    Server();
    Server(const std::string& initPort, const std::string& initPassword);
    ~Server();

    // Entry points
    void startServer();
    void runMainLoop();

private:
    // --- Socket Setup & Configuration ---
    void prepareSocketConfiguration(struct addrinfo& config);
    void resolveLocalAddress(struct addrinfo& config, struct addrinfo*& addressList);
    int createSocket(struct addrinfo* addressList);
    void bindSocketToAddress(int fd, struct addrinfo* addressList);
    void enableListening(int fd, struct addrinfo* addressList);
    std::string generateCurrentTime(const std::string& timeFormat);

    // --- Poll Loop Core ---
    void preparePollingList();
    void evaluatePollEvents();
    void processEachPollfd();
    void checkPollfdState(std::vector<struct pollfd>::iterator descriptor);
    void handleIncomingOrOutgoingData(std::vector<struct pollfd>::iterator descriptor);
    void handlePollFailure();
    void handleConnectionIssue(std::vector<struct pollfd>::iterator descriptor);

    // --- New Client Handling ---
    void acceptNewClient();
    void registerClientPollfd(int clientSocket);
    void createClientInstance(int clientSocket, const struct sockaddr_storage& clientAddress);

    // --- Client Communication ---
    void handleClientCommunication(std::vector<struct pollfd>::iterator descriptor);
    User* getUserByFd(int fd);
    void logReceiveStart();
    bool isDisconnection(int bytesRead);
    void handleDisconnection(User* user, struct pollfd& pollEntry, int resultCode);
    void logUserNotFound(int fd);
    void logByteCount(int bytes);

    // --- Socket Reading ---
    int receiveClientData(User* user);
    int readCompleteMessageFromSocket(int socketFd, std::string& outputMessage);
    void printSocketReadConfirmation();
    void printUserMessageLog(User* sender, const std::string& content);

    // --- Disconnection & Cleanup ---
    void shutdownGracefully();
    void cleanupAllResources();
    void clearAllUsers();
    void closeAllPollDescriptors();
    void deleteAllChannels();

    // --- User Termination ---
    void terminateUserSession(User* userObj, struct pollfd& pollEntry);
    void removeUserByPollfd(struct pollfd& socketEntry);
    User* findUserBySocket(int socketId);
    void closeSocketAndErasePollfd(int socketId);
    void eraseUserFromUserList(int socketId);

    // --- Channel Detachment ---
    void detachUserFromAllChannels(User* userPtr);
    void removeUserRolesFromChannel(Channel* chan1, const std::string& nick);
    bool isChannelEmpty(Channel* chan1);

    // --- Authentication Failure ---
    void disconnectUnauthenticatedClient(User* disconnectedClient);
    void removeSocketFromPollList(int clientSocket);
    void removeUserEntry(int clientSocket);

    // --- Command Initialization ---
    void setupCommandHandlers();

private:
    std::string portValue;
    std::string passwordValue;
    int mainSocketDescriptor;
    std::string serverCreatedAt;

    std::vector<struct pollfd> listOfPollDescriptors;
    std::vector<std::pair<int, User*> > listOfUsers;
    std::vector<std::pair<std::string, Channel*> > listOfChannels;
};

extern int isServerActive;

#endif

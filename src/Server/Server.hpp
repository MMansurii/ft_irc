#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel/Channel.hpp"
#include "Client/Client.hpp"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <poll.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

class Server {
public:
  Server();
  Server(const std::string &initPort, const std::string &initPassword);
  ~Server();

  // Entry points
  void startServer();
  void runMainLoop();

private:
  // --- Socket Setup & Configuration ---
  void prepareSocketConfiguration(struct addrinfo &config);
  void resolveLocalAddress(struct addrinfo &config,
                           struct addrinfo *&addressList);
  int createSocket(struct addrinfo *addressList);
  void bindSocketToAddress(int fd, struct addrinfo *addressList);
  void enableListening(int fd, struct addrinfo *addressList);
  std::string generateCurrentTime(const std::string &timeFormat);

  // --- Poll Loop Core ---
  void preparePollingList();
  void evaluatePollEvents();
  void processEachPollfd();
  void checkPollfdState(std::vector<struct pollfd>::iterator descriptor);
  void
  handleIncomingOrOutgoingData(std::vector<struct pollfd>::iterator descriptor);
  void handlePollFailure();
  void handleConnectionIssue(std::vector<struct pollfd>::iterator descriptor);
  // Forceful client disconnection handler
  void
  terminateClientConnection(std::vector<struct pollfd>::iterator descriptor,
                            const std::string &message);

  // --- New Client Handling ---
  void acceptNewClient();
  void registerClientPollfd(int clientSocket);
  // Initialize a new client with given socket
  void createClientInstance(int clientSocket);

  // --- Client Communication ---
  void
  handleClientCommunication(std::vector<struct pollfd>::iterator descriptor);
  Client *getUserByFd(int fd);
  void logReceiveStart();
  bool isDisconnection(int bytesRead);
  void handleDisconnection(Client *user, struct pollfd &pollEntry,
                           int resultCode);
  void logUserNotFound(int fd);
  void logByteCount(int bytes);

  // --- Socket Reading ---
  int receiveClientData(Client *user);
  int readCompleteMessageFromSocket(int socketFd, std::string &outputMessage);
  void printSocketReadConfirmation();
  void printUserMessageLog(Client *sender, const std::string &content);

  // Parse and execute a client command
  void interpretClientCommand(Client *user);

  // --- Disconnection & Cleanup ---
  void shutdownGracefully();
  void cleanupAllResources();
  void clearAllUsers();
  void closeAllPollDescriptors();
  void deleteAllChannels();

  // --- User Termination ---
  void terminateUserSession(Client *userObj, struct pollfd &pollEntry);
  void removeUserByPollfd(struct pollfd &socketEntry);
  Client *findUserBySocket(int socketId);
  void closeSocketAndErasePollfd(int socketId);
  void eraseUserFromUserList(int socketId);

  // --- Channel Detachment ---
  void detachUserFromAllChannels(Client *userPtr);

  // --- Authentication Failure ---
  void disconnectUnauthenticatedClient(Client *disconnectedClient);
  void removeSocketFromPollList(int clientSocket);
  void removeUserEntry(int clientSocket);

private:
  std::string portValue;
  std::string passwordValue;
  int mainSocketDescriptor;
  std::string serverCreatedAt;

  std::vector<struct pollfd> listOfPollDescriptors;
  std::vector<std::pair<int, Client *>> listOfUsers;
  std::vector<std::pair<std::string, Channel *>> listOfChannels;
};

extern int isServerActive;

#endif

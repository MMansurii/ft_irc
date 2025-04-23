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
#include <sstream>
#include <functional>
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
  Client *getClientByFd(int fd);
  void logReceiveStart();
  bool isDisconnection(int bytesRead);
  void handleDisconnection(Client *client, struct pollfd &pollEntry,
                           int resultCode);
  void logClientNotFound(int fd);
  void logByteCount(int bytes);

  // --- Socket Reading ---
  int receiveClientData(Client *client);
  int readCompleteMessageFromSocket(int socketFd, std::string &outputMessage);
  void printSocketReadConfirmation();
  void printClientMessageLog(Client *sender, const std::string &content);

  // Command dispatch: map IRC verb to handler
  typedef void (Server::*CmdHandler)(Client* client, std::istringstream& iss, const std::string& line);
  std::map<std::string, CmdHandler> cmdHandlers;
  // Populate cmdHandlers map
  void setupCommandHandlers();
  // Individual command handlers
  void handleCAP(Client* client, std::istringstream& iss, const std::string& line);
  void handlePASS(Client* client, std::istringstream& iss, const std::string& line);
  void handleNICK(Client* client, std::istringstream& iss, const std::string& line);
  void handleUSER(Client* client, std::istringstream& iss, const std::string& line);
  void handlePING(Client* client, std::istringstream& iss, const std::string& line);
  void handleQUIT(Client* client, std::istringstream& iss, const std::string& line);
  void handleJOIN(Client* client, std::istringstream& iss, const std::string& line);
  void handlePART(Client* client, std::istringstream& iss, const std::string& line);
  void handlePRIVMSG(Client* client, std::istringstream& iss, const std::string& line);
  void handleKICK(Client* client, std::istringstream& iss, const std::string& line);
  void handleINVITE(Client* client, std::istringstream& iss, const std::string& line);
  void handleMODE(Client* client, std::istringstream& iss, const std::string& line);
  void handleTOPIC(Client* client, std::istringstream& iss, const std::string& line);

  // Parse and execute a client command
  void interpretClientCommand(Client *client);

  // --- Disconnection & Cleanup ---
  void shutdownGracefully();
  void cleanupAllResources();
  void clearAllClients();
  void closeAllPollDescriptors();
  void deleteAllChannels();

  // --- Client Termination ---
  void terminateClientSession(Client *clientObj, struct pollfd &pollEntry);
  void removeClientByPollfd(struct pollfd &socketEntry);
  Client *findClientBySocket(int socketId);
  void closeSocketAndErasePollfd(int socketId);
  void eraseClientFromClientList(int socketId);

  // --- Channel Detachment ---
  void detachClientFromAllChannels(Client *clientPtr);

  // --- Authentication Failure ---
  void disconnectUnauthenticatedClient(Client *disconnectedClient);
  void removeSocketFromPollList(int clientSocket);
  void removeClientEntry(int clientSocket);

private:
  std::string portValue;
  std::string passwordValue;
  int mainSocketDescriptor;
  std::string serverCreatedAt;

  std::vector<struct pollfd> listOfPollDescriptors;
  std::vector<std::pair<int, Client *>> listOfClients;
  std::vector<std::pair<std::string, Channel *>> listOfChannels;
};

extern int isServerActive;

#endif

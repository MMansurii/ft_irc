
#include "Server.hpp"
#include <array>

int isServerActive; // Control server loop

Server::Server(std::string port) : portValue(port), mainSocketDescriptor(-1) {}

// --> 101
//initialize and configure the server socket
void Server::startServer() {
    struct addrinfo socketConfig, *addressList = NULL;
    prepareSocketConfiguration(socketConfig);
    resolveLocalAddress(socketConfig, addressList);

    int serverSocket = createSocket(addressList);
    bindSocketToAddress(serverSocket, addressList);
    enableListening(serverSocket, addressList);

    mainSocketDescriptor = serverSocket;
    freeaddrinfo(addressList);
    serverCreatedAt = generateCurrentTime("%Y-%m-%d.%X");
}

// Prepare the configuration structure used by getaddrinfo
// ai_family: AF_UNSPEC allows both IPv4 and IPv6
// ai_socktype: SOCK_STREAM indicates a TCP connection
// ai_flags: AI_PASSIVE indicates use of wildcard local IP
void Server::prepareSocketConfiguration(struct addrinfo &config) {
    std::memset(&config, 0, sizeof(config));
    config.ai_family = AF_UNSPEC;      // Accept both IPv4 and IPv6
    config.ai_socktype = SOCK_STREAM;  // TCP socket
    config.ai_flags = AI_PASSIVE;      // Bind to local IP (0.0.0.0 or ::)
}

// Uses getaddrinfo to resolve local IP and port for binding
// Returns a linked list of address structures in addressList
void Server::resolveLocalAddress(struct addrinfo &config, struct addrinfo *&addressList) {
    int status = getaddrinfo(NULL, portValue.c_str(), &config, &addressList);
    if (status != 0) {
        std::cerr << "\033[31m Error: Failed to resolve address — " << gai_strerror(status) << " \033[0m\n";
        exit(1);
    }
    std::cout << "\033[32m [OK] \033[0m\033[90m Address info resolved successfully \033[0m\n";
}

// Creates a socket using the info from getaddrinfo
// Returns the socket file descriptor if successful
int Server::createSocket(struct addrinfo *addressList) {
    int fd = socket(addressList->ai_family, addressList->ai_socktype, addressList->ai_protocol);
    if (fd == -1) {
        std::cerr << "\033[31m Error: Could not create socket \033[0m\n";
        freeaddrinfo(addressList);
        exit(1);
    }
    std::cout << "\033[32m [OK] \033[0m\033[90m Socket created \033[0m\n";
    return fd;
}

// Binds the socket to the resolved address (IP and port)
// Exits if binding fails
void Server::bindSocketToAddress(int fd, struct addrinfo *addressList) {
    if (bind(fd, addressList->ai_addr, addressList->ai_addrlen) == -1) {
        std::cerr << "\033[31m Error: Could not bind socket to address \033[0m\n";
        close(fd);
        freeaddrinfo(addressList);
        exit(1);
    }
    std::cout << "\033[32m [OK] \033[0m\033[90m Socket successfully bound to address \033[0m\n";
}

// Puts the socket into listening mode so it can accept connections
// The backlog is set to 1000 pending connections
void Server::enableListening(int fd, struct addrinfo* addressList) {
    if (listen(fd, 1000) == -1) {
        std::cerr << "\033[31m Error: Failed to start listening on socket \033[0m\n";
        close(fd);
        freeaddrinfo(addressList); // Clean up
        exit(1);
    }
    std::cout << "\033[32m [OK] \033[0m\033[90m Socket is now listening for connections \033[0m\n";
}

// Generates current local time as string in given format (e.g. "YYYY-MM-DD.HH:MM:SS")
std::string Server::generateCurrentTime(const std::string& timeFormat) 
{
    time_t currentTime = time(nullptr);
    struct tm localTimeInfo = *localtime(&currentTime);
    
    std::array<char, 100> formattedBuffer{};
    strftime(formattedBuffer.data(), formattedBuffer.size(), 
             timeFormat.c_str(), &localTimeInfo);

    std::cout << "\033[33m[ SERVER CREATED ]\033[0m\n\n";
    return std::string(formattedBuffer.data());
}
// <-- 101

// Florent
// Maps command strings to their corresponding handler methods
void Server::setupCommandHandlers() {
    // commandHandlers.push_back(std::make_pair("CAP", &Server::handleCapCommand));
    // commandHandlers.push_back(std::make_pair("PASS", &Server::handlePassCommand));
    // commandHandlers.push_back(std::make_pair("NICK", &Server::handleNickCommand));
    // commandHandlers.push_back(std::make_pair("USER", &Server::handleUserCommand));
    // commandHandlers.push_back(std::make_pair("QUIT", &Server::handleQuitCommand));
    // commandHandlers.push_back(std::make_pair("PING", &Server::handlePingCommand));
    // commandHandlers.push_back(std::make_pair("JOIN", &Server::handleJoinCommand));
    // commandHandlers.push_back(std::make_pair("INVITE", &Server::handleInviteCommand));
    // commandHandlers.push_back(std::make_pair("PRIVMSG", &Server::handlePrivMsgCommand));
    // commandHandlers.push_back(std::make_pair("KICK", &Server::handleKickCommand));
    // commandHandlers.push_back(std::make_pair("MODE", &Server::handleModeCommand));
    // commandHandlers.push_back(std::make_pair("TOPIC", &Server::handleTopicCommand));
    // commandHandlers.push_back(std::make_pair("WHO", &Server::handleWhoCommand));
    // commandHandlers.push_back(std::make_pair("NOTICE", &Server::handleNoticeCommand));
}


// -->107
// Starts the main server loop: monitors connections and routes activity
void Server::runMainLoop() {
    preparePollingList(); // Register main socket for polling

    while (isServerActive) {
        evaluatePollEvents(); // Poll for network activity
    }

    shutdownGracefully(); // Clean exit once loop ends
}

// Registers the server's primary socket into the poll list
void Server::preparePollingList() {

// typedef struct pollfd {
//     int   fd;
//     short events;
//     short revents;
// } pollfd;
    struct pollfd serverPollfds;
    serverPollfds.fd = mainSocketDescriptor;
    serverPollfds.events = POLLIN | POLLOUT;
    serverPollfds.revents = 0;
    listOfPollDescriptors.push_back(serverPollfds);

    std::cout << "\033[32m[ Server is now listening on socket: " << mainSocketDescriptor << " ]\033[0m\n";
}

// Runs poll() and handles the result status
void Server::evaluatePollEvents() {
    int activeCount = poll(listOfPollDescriptors.data(), listOfPollDescriptors.size(), -1);

    if (activeCount == -1) {
        handlePollFailure(); // poll failed
        return;
    }

    if (activeCount > 0) {
        processEachPollfd(); // Check each fd for readiness
    }
}

// Prints poll error and sets shutdown flag if critical
void Server::handlePollFailure() {
    std::cerr << "\033[31m[ Poll failure: " << strerror(errno) << " ]\033[0m\n";

    if (errno == EFAULT || errno == ENOMEM) {
        // isServerActive = false; // Critical error, stop server
    }
}

// Walks through each pollfd to see who has activity
void Server::processEachPollfd() {
    for (std::vector<struct pollfd>::iterator currentD = listOfPollDescriptors.begin(); currentD != listOfPollDescriptors.end(); ) {
        if (currentD->revents == 0) {
            ++currentD;
            continue;
        }
        checkPollfdState(currentD); // Dispatch based on revents
        break; // Only one per tick to avoid iterator invalidation
    }
}

// Routes pollfd activity to the correct handler
void Server::checkPollfdState(std::vector<struct pollfd>::iterator descriptor) {
    if ((descriptor->revents & POLLIN) || (descriptor->revents & POLLOUT)) {
        handleIncomingOrOutgoingData(descriptor); // Read/write event
    } else if ((descriptor->revents & POLLERR) || (descriptor->revents & POLLHUP) || (descriptor->revents & POLLNVAL)) {
        handleConnectionIssue(descriptor); // Error or disconnection
    }
}

// Delegates event type: accept new client or read client data

void Server::handleIncomingOrOutgoingData(std::vector<struct pollfd>::iterator descriptor) {
    if (descriptor->fd == mainSocketDescriptor) {
        acceptNewClient(); // Incoming client
    } else {
        handleClientCommunication(descriptor); // Existing client
    }
}

// Reports and manages faulty or dropped connections
void Server::handleConnectionIssue(std::vector<struct pollfd>::iterator descriptor) {
    std::string issueMessage = "Unknown issue";

    if (descriptor->revents & POLLHUP) {
        issueMessage = "Client closed the connection";
    }
    else if (descriptor->revents & POLLERR) {
        issueMessage = "Socket-level error detected";
    }
    else if (descriptor->revents & POLLNVAL) {
        issueMessage = "Bad socket descriptor detected";
    }

    terminateClientConnection(descriptor, issueMessage);
}

// Forcefully removes a user after disconnection or error
void Server::terminateClientConnection(std::vector<struct pollfd>::iterator descriptor, const std::string& message) {
    std::cerr << "\033[31m[ Client disconnected: " << message << " (fd: " << descriptor->fd << ") ]\033[0m\n";

    try {
        for (std::vector<std::pair<int, User*> >::iterator it = listOfUsers.begin(); it != listOfUsers.end(); ++it) {
            if (it->first == descriptor->fd) {
                leaveAllUserChannels(it->second);
                delete it->second;
                listOfUsers.erase(it);
                break;
            }
        }
        close(descriptor->fd);
        listOfPollDescriptors.erase(descriptor);
    } catch (const std::exception& err) {
        std::cerr << "\033[31m[ Cleanup failure: " << err.what() << " ]\033[0m\n";
    }
}
// <-- 107
// 


// --> 28
/////   acceptNewClient(); // Incoming client
// Accepts a new client connection and registers them
void Server::acceptNewClient() {
    struct sockaddr_storage clientAddress;
    socklen_t addressLength = sizeof(clientAddress);

    int clientSocket = accept(mainSocketDescriptor,
                              reinterpret_cast<sockaddr*>(&clientAddress),
                              &addressLength);

    if (clientSocket == -1) {
        std::cerr << "\033[31m[ Accept failed: could not establish new connection ]\033[0m\n";
        return;
    }

    registerClientPollfd(clientSocket);
    createClientInstance(clientSocket, clientAddress);

    std::cout << "\033[32m[ New connection accepted ]\033[0m\n";
    std::cout << "\033[33m[ USER LOGGED IN ]\033[0m\n\n";
}

// Adds the new client's socket to the polling list
void Server::registerClientPollfd(int clientSocket) {
    struct pollfd newPollfd;
    newPollfd.fd = clientSocket;
    newPollfd.events = POLLIN;
    newPollfd.revents = 0;

    listOfPollDescriptors.push_back(newPollfd);
}

// Creates a new User object and registers it
void Server::createClientInstance(int clientSocket, const struct sockaddr_storage& clientAddress) {
    // Check if this client socket is already registered
    for (size_t i = 0; i < listOfUsers.size(); ++i) {
        if (listOfUsers[i].first == clientSocket) {
            std::cerr << "\033[31m[ Duplicate socket FD detected: " << clientSocket << " — Skipping registration ]\033[0m\n";
            return;
        }
    }

    // Create new User and add to user list
    User* newUser = new User(clientSocket, const_cast<sockaddr_storage*>(&clientAddress));
    listOfUsers.push_back(std::make_pair(clientSocket, newUser));
}
/// end acceptNewClient(); // Incoming client 
// <-- 28

// --> 31
// reads data from user's socket and stores it in their message buffer
int Server::receiveClientData(User* user) {
    int userSocket = user->getUserFd();
    std::string fullMessage;
    
    int lastBytesRead = readCompleteMessageFromSocket(userSocket, fullMessage);

    user->clearMessage();                    // Clear old message buffer
    user->setMessage(fullMessage);           // Save the newly received message

    printSocketReadConfirmation();
    printUserMessageLog(user, fullMessage);


    return lastBytesRead;                    // Return last read size (as in original)
}

// Reads data from a socket into a string until "\r\n" is received
int Server::readCompleteMessageFromSocket(int socketFd, std::string& outputMessage) {
    int lastBytesRead = 0;
    char tempBuffer[1024];

    while (outputMessage.rfind("\r\n") != outputMessage.length() - 2 || outputMessage.length() < 2) {
        std::memset(tempBuffer, 0, sizeof(tempBuffer));
        lastBytesRead = recv(socketFd, tempBuffer, sizeof(tempBuffer), 0);
        if (lastBytesRead <= 0)
            break;
        outputMessage.append(tempBuffer);
    }

    return lastBytesRead;
}


// Displays confirmation that data was read from the socket
void Server::printSocketReadConfirmation() {
    std::cout << "\033[32m[ Data successfully read from socket ]\033[0m\033[90m (recv OK)\033[0m\n";
}

// Displays the incoming message received from a specific user
void Server::printUserMessageLog(User* sender, const std::string& content) {
    std::cout << "\033[33m[ INCOMING MESSAGE ] From: "
              << sender->getNickname() << " | Content: " << content
              << " \033[0m\n\n";
}

// <-- 31


// --> 199
// Reads data from a client and processes it
void Server::handleClientCommunication(std::vector<struct pollfd>::iterator descriptor) {
    try {
        User* user = getUserByFd(descriptor->fd);
        if (!user) return;

        int bytes = 0;
        std::string message = readFromSocket(user->getUserFd(), bytes);

        if (isClientDisconnected(bytes)) {
            std::string reason = (bytes == 0) ? "Client hung up" : "recv() error";
            std::cerr << "\033[31m[ Disconnecting: " << reason << " ]\033[0m\n";
            terminateUserSession(user, *descriptor);
            return;
        }

        processClientMessage(user, bytes, message);
    }
    catch (const std::out_of_range& e) {
        std::cerr << "\033[31m[ Error: Out-of-range while handling pollfd " << descriptor->fd << " ]\033[0m\n";
    }
    catch (const std::exception& e) {
        std::cerr << "\033[31m[ Error: Exception in handleClientCommunication: " << e.what() << " ]\033[0m\n";
    }
}

// Retrieves a user from the active user list by socket file descriptor
User* Server::getUserByFd(int fd) {
    for (size_t i = 0; i < listOfUsers.size(); ++i) {
        if (listOfUsers[i].first == fd)
            return listOfUsers[i].second;
    }
    return nullptr;
}

// Checks if the socket has disconnected or encountered an error
bool Server::isClientDisconnected(int byteCount) {
    return byteCount <= 0;
}

// Reads data from a user's socket until a full message is received
std::string Server::readFromSocket(int fd, int& byteCount) {
    std::string fullMessage;
    char buffer[1024];
    byteCount = 0;

    while (fullMessage.rfind("\r\n") != fullMessage.length() - 2 || fullMessage.length() < 2) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            byteCount = bytes;
            break;
        }
        fullMessage.append(buffer);
        byteCount += bytes;
    }

    return fullMessage;
}

// Sets user message buffer, logs the message, and triggers command parsing
void Server::processClientMessage(User* user, int bytes, const std::string& message) {
    std::cout << "\033[90mBytes received: " << bytes << "\033[0m\n";
    user->setMessage(message);
    logMessage(user, message);
    interpretClientCommand(user);  // Replaces _parseCmd()
}

// Logs incoming user messages with formatting
void Server::logMessage(User* user, const std::string& msg) {
    std::cout << "\033[33m[ MESSAGE RECEIVED : "
              << user->getNickname() << " => " << msg << " ]\033[0m\n\n";
}

// end handleClientCommunication(struct pollfd& descriptor)



///*****     */


// --> 59
// Gracefully closes the server and prints status
void Server::shutdownGracefully() {
    cleanupAllResources();
    std::cerr << "\n\033[33m[ Server has been shut down safely ]\033[0m\n";
}

// Frees users, closes fds, and clears tracking lists
// Disconnect and clean all users, file descriptors, and channels
void Server::cleanupAllResources() {
    clearAllUsers();
    closeAllPollDescriptors();
    deleteAllChannels();
}

// Delete all User objects and close their sockets
void Server::clearAllUsers() {
    for (size_t i = 0; i < listOfUsers.size(); ++i) {
        close(listOfUsers[i].first);
        delete listOfUsers[i].second;
    }
    listOfUsers.clear();
}

// Close all poll file descriptors
void Server::closeAllPollDescriptors() {
    for (size_t i = 0; i < listOfPollDescriptors.size(); ++i) {
        close(listOfPollDescriptors[i].fd);
    }
    listOfPollDescriptors.clear();
}

// Delete all channels stored on the server
void Server::deleteAllChannels() {
    for (size_t i = 0; i < listOfChannels.size(); ++i) {
        delete listOfChannels[i].second;
    }
    listOfChannels.clear();
}

// <-- 59


//** */



// --> 61
void Server::terminateUserSession(User* userObj, struct pollfd& pollEntry) {
    

    detachUserFromAllChannels(userObj);     // remove from channels
    removeUserByPollfd(pollEntry);
}

// Detaches the given user from all IRC channels and removes empty channels
void Server::detachUserFromAllChannels(User* userPtr) {
    std::string userNick = userPtr->getNickname();

    for (size_t i = 0; i < listOfChannels.size(); ++i) {
        Channel* chan1 = listOfChannels[i].second;

        removeUserRolesFromChannel(chan1, userNick);

        if (isChannelEmpty(chan1)) {
            delete chan1;
            listOfChannels.erase(listOfChannels.begin() + i);
            --i; // Adjust index after deletion
        }
    }
}

// Removes all roles the user had in a specific channel
void Server::removeUserRolesFromChannel(Channel* chan1, const std::string& nick) {
    if (chan1->foundOperator(nick)) {
        chan1->kickModeOperator(nick);
        chan1->setNbUser(-1);
    }

    if (chan1->foundUser(nick)) {
        chan1->kickModeUser(nick);
        chan1->setNbUser(-1);
    }

    if (chan1->foundInvited(nick)) {
        chan1->kickModeInvited(nick);
    }
}

// Determines whether the given channel is empty
bool Server::isChannelEmpty(Channel* chan1) {
    return chan1->getNbUser() <= 0;
}

// <-- 61










// --> method that deletes a user based on their pollfd struct 63
void Server::removeUserByPollfd(struct pollfd& socketEntry) {
    try {
        int socketId = socketEntry.fd;
        std::cout << "\033[32m[ Removing user with socket fd: " << socketId << " ]\033[0m\n";

        User* targetUser = findUserBySocket(socketId);
        if (!targetUser) {
            std::cerr << "\033[31m[ No user found for socket fd: " << socketId << " ]\033[0m\n";
            return;
        }

        closeSocketAndErasePollfd(socketId);
        eraseUserFromUserList(socketId);

        std::cerr << "\033[31m[ User disconnected on socket: " << socketId << " ]\033[0m\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "\033[31m[ Exception during user removal: " << ex.what() << " ]\033[0m\n";
    }
}

// Searches for a user object given its socket ID
User* Server::findUserBySocket(int socketId) {
    for (size_t i = 0; i < listOfUsers.size(); ++i) {
        if (listOfUsers[i].first == socketId) {
            return listOfUsers[i].second;
        }
    }
    return nullptr;
}

// Closes the socket and removes the corresponding pollfd entry
void Server::closeSocketAndErasePollfd(int socketId) {
    if (socketId != -1) {
        close(socketId);
    }
    for (size_t j = 0; j < listOfPollDescriptors.size(); ++j) {
        if (listOfPollDescriptors[j].fd == socketId) {
            listOfPollDescriptors.erase(listOfPollDescriptors.begin() + j);
            break;
        }
    }
}


// Deletes the user object and removes it from the user list
void Server::eraseUserFromUserList(int socketId) {
    for (size_t i = 0; i < listOfUsers.size(); ++i) {
        if (listOfUsers[i].first == socketId) {
            delete listOfUsers[i].second;
            listOfUsers.erase(listOfUsers.begin() + i);
            break;
        }
    }
}

// <-- method that deletes a user based on their pollfd struct 63
// --> disconnects a client that failed authentication 67

void Server::disconnectUnauthenticatedClient(User* disconnectedClient) {
    try {
        disconnectedClient->sendReply("Please, enter PASS first. Disconnecting.");
        int clientSocket = disconnectedClient->getUserFd();

        close(clientSocket);
        removeSocketFromPollList(clientSocket);
        removeUserEntry(clientSocket);

        std::cout << "\033[33m[ Unauthenticated client removed: fd " << clientSocket << " ]\033[0m\n";
    }
    catch (const std::exception& error) {
        std::cerr << "\033[31m[ Exception during disconnect: " << error.what() << " ]\033[0m\n";
    }
}

// removes pollfd associated with the given socket
void Server::removeSocketFromPollList(int clientSocket) {
    for (std::vector<struct pollfd>::iterator socketEntry = listOfPollDescriptors.begin();
         socketEntry != listOfPollDescriptors.end(); ++socketEntry) {
        if (socketEntry->fd == clientSocket) {
            listOfPollDescriptors.erase(socketEntry);
            break;
        }
    }
}

// finds and removes the user from the active user list by socket
void Server::removeUserEntry(int clientSocket) {
    for (std::vector<std::pair<int, User*> >::iterator userEntry = listOfUsers.begin();
         userEntry != listOfUsers.end(); ++userEntry) {
        if (userEntry->first == clientSocket) {
            delete userEntry->second;
            listOfUsers.erase(userEntry);
            break;
        }
    }
}


// <-- disconnects a client that failed authentication 67
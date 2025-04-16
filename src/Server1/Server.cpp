
#include "Server.hpp"
#include <array>

 Server::Server(std::string port) : portValue(port), listeningSocket(-1) {}

// Main function to initialize and configure the server socket
void Server::startServer() {
    struct addrinfo socketConfig, *addressList = NULL;
    prepareSocketConfiguration(socketConfig);
    resolveLocalAddress(socketConfig, addressList);

    int serverSocket = createSocket(addressList);
    bindSocketToAddress(serverSocket, addressList);
    enableListening(serverSocket, addressList);

    listeningSocket = serverSocket;
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



////////////////
#include "Server.hpp"

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
    struct pollfd serverPollfd;
    serverPollfd.fd = mainSocketDescriptor;
    serverPollfd.events = POLLIN | POLLOUT;
    listOfPollDescriptors.push_back(serverPollfd);

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
        isServerActive = false; // Critical error, stop server
    }
}

// Walks through each pollfd to see who has activity
void Server::processEachPollfd() {
    for (std::vector<struct pollfd>::iterator it = listOfPollDescriptors.begin(); it != listOfPollDescriptors.end(); ) {
        if (it->revents == 0) {
            ++it;
            continue;
        }
        checkPollfdState(it); // Dispatch based on revents
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
        readClientData(descriptor); // Existing client
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

// Gracefully closes the server and prints status
void Server::shutdownGracefully() {
    cleanupAllResources();
    std::cerr << "\n\033[33m[ Server has been shut down safely ]\033[0m\n";
}

// Frees users, closes fds, and clears tracking lists
void Server::cleanupAllResources() {
    for (std::vector<std::pair<int, User*> >::iterator it = listOfUsers.begin(); it != listOfUsers.end(); ++it) {
        leaveAllUserChannels(it->second);
        close(it->first);
        delete it->second;
    }
    listOfUsers.clear();
    listOfPollDescriptors.clear();
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

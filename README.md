# ft\_irc

## Introduction

**ft\_irc** is an IRC (Internet Relay Chat) server implemented in C, following the RFC 1459 standard. This project is part of the 42 school curriculum and aims to develop a deeper understanding of network programming, socket communication, and multi-client management.

## Features

- Supports multiple clients using non-blocking sockets
- Implements core IRC commands (JOIN, PART, PRIVMSG, KICK, etc.)
- Manages user authentication and nicknames
- Allows channel creation and moderation
- Handles client disconnections and errors gracefully

## Installation

### Prerequisites

- Linux or macOS
- GCC compiler
- Make

### Clone Repository

```sh
git clone https://github.com/yourusername/ft_irc.git
cd ft_irc
```

### Build the Project

```sh
make
```

## Usage

### Start the Server

```sh
./irc_server <port> <password>
```

- `<port>`: The port number for the server to listen on.
- `<password>`: (Optional) Server password for clients to connect.

### Connect a Client

You can use an IRC client like `netcat` or `WeeChat`:

```sh
nc localhost <port>
```

Or using an official IRC client:

```sh
/connect localhost <port>
```

## Project Structure

```
ft_irc/
├── src/
│   ├── main.c
│   ├── server.c
│   ├── client.c
│   ├── commands.c
│   └── channel.c
├── include/
│   ├── server.h
│   ├── client.h
│   ├── commands.h
│   └── channel.h
├── Makefile
└── README.md
```

## Contribution

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Commit your changes (`git commit -m 'Add new feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Open a pull request.

## License

This project is licensed under the MIT License.

## Authors

- **Your Name** - [GitHub Profile](https://github.com/yourusername)

## References

- [RFC 1459 - Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)


// Server/commands/USER.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC USER command
void Server::handleUSER(Client* client, std::istringstream& iss, const std::string& line) {
  std::string clientnm, mode, unused;
  size_t pos = line.find(" :");
  std::string real;
  if (pos != std::string::npos)
    real = line.substr(pos + 2);
  iss >> clientnm >> mode >> unused;
  if (clientnm.empty() || real.empty()) {
    client->do_TMess("461 USER :Not enough parameters", 2);
    return;
  }
  client->setCl_str_info(0, clientnm);
  client->setCl_str_info(3, real);
  // After PASS, NICK, and USER, send welcome numerics once
  if (client->getCl_int_info(0) && !client->getCl_str_info(1).empty() && client->getCl_int_info(1) == 0) {
    std::string nick = client->getCl_str_info(1);
    std::string uname = client->getCl_str_info(0);
    std::string host = client->getCl_str_info(2);
    std::string srv = client->getCl_str_info(4);
    client->do_TMess(
      ":" + srv + " 001 " + nick +
      " :Welcome to the Internet Relay Network " + nick + "!" + uname + "@" + host
      , 2);
    client->do_TMess(
      ":" + srv + " 002 " + nick +
      " :Your host is " + srv + ", running version ft_irc-1.0"
      , 2);
    client->do_TMess(
      ":" + srv + " 003 " + nick +
      " :This server was created " + serverCreatedAt
      , 2);
    client->do_TMess(
      ":" + srv + " 004 " + nick + " " + srv +
      " ft_irc-1.0 oiwszchrbeDF"
      , 2);
    client->setCl_int_info(1, 1);
  }
}
// Server/commands/USER.cpp
#include "../Server.hpp"
#include <sstream>
#include <string>

// Handle the IRC USER command
void Server::handleUSER(Client* user, std::istringstream& iss, const std::string& line) {
  std::string usernm, mode, unused;
  size_t pos = line.find(" :");
  std::string real;
  if (pos != std::string::npos)
    real = line.substr(pos + 2);
  iss >> usernm >> mode >> unused;
  if (usernm.empty() || real.empty()) {
    user->sendReply("461 USER :Not enough parameters");
    return;
  }
  user->setCl_str_info(0, usernm);
  user->setCl_str_info(3, real);
  // After PASS, NICK, and USER, send welcome numerics once
  if (user->getCl_int_info(0) && !user->getNickname().empty() && user->getCl_int_info(1) == 0) {
    std::string nick = user->getNickname();
    std::string uname = user->getCl_str_info(0);
    std::string host = user->getCl_str_info(2);
    std::string srv = user->getCl_str_info(4);
    user->sendReply(
      ":" + srv + " 001 " + nick +
      " :Welcome to the Internet Relay Network " + nick + "!" + uname + "@" + host
    );
    user->sendReply(
      ":" + srv + " 002 " + nick +
      " :Your host is " + srv + ", running version ft_irc-1.0"
    );
    user->sendReply(
      ":" + srv + " 003 " + nick +
      " :This server was created " + serverCreatedAt
    );
    user->sendReply(
      ":" + srv + " 004 " + nick + " " + srv +
      " ft_irc-1.0 oiwszchrbeDF"
    );
    user->setCl_int_info(1, 1);
  }
}
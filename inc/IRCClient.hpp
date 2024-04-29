#ifndef IRC_CLIENT_HPP
#define IRC_CLIENT_HPP

#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>
#include <vector>
#include "ChannelInfo.hpp"
#include "IRCServer.hpp"

class IRCClient {
public:
    int socket;
    std::string nick;
    std::string user;
	std::string realname;
    std::string host;
	std::string mode;

    std::vector<ChannelInfo> channels;
	typedef void (IRCClient::*CommandHandler)(const std::string&);
    std::map<std::string, CommandHandler> commandHandlers;
	IRCServer* server;

	IRCClient();
	IRCClient(int sock, const std::string& host);

	void setServer(IRCServer* server);
    bool runCommand(const std::string& buffer);
	void setNick(const std::string& nick);
	void handleNick(const std::string& params);
	void handleUser(const std::string& params);
	void handleJoin(const std::string& params);
	void setUserDetails(const std::string& username, const std::string& mode, const std::string& servername, const std::string& realname);
	std::string getNickValue();
	void joinChannel(const std::string& channelName, const std::string& password);
	void initCommandHandlers();
};

#endif
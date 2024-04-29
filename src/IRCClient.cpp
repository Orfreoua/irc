#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include "IRCClient.hpp"
#include <map>
#include <sstream>

// Default constructor
IRCClient::IRCClient() 
    : socket(-1), nick(""), user("defaultUser"), realname(""), host(""), mode("") {
    initCommandHandlers(); // Initialize command handlers
}

// Parameterized constructor with socket and host
IRCClient::IRCClient(int sock, const std::string& host)
    : socket(sock), nick(""), user("defaultUser"), realname(""), host(host), mode("") {
    initCommandHandlers(); // Initialize command handlers
}

// Setter for server
void IRCClient::setServer(IRCServer* server) {
    this->server = server;
}

// Setter for nickname
void IRCClient::setNick(const std::string& nick) {
	this->nick = nick;
}

void IRCClient::initCommandHandlers() {
	// ? Client Commands
	commandHandlers["NICK"] = &IRCClient::handleNick;
	commandHandlers["USER"] = &IRCClient::handleUser;
	commandHandlers["JOIN"] = &IRCClient::handleJoin;
	// TODO :commandHandlers["PRIVMSG"] = &IRCClient::handlePrivMsg;
	// TODO :commandHandlers["PART"] = &IRCClient::handlePart;
	// TODO :commandHandlers["MODE"] = &IRCClient::handleMode;
	// TODO :commandHandlers["QUIT"] = &IRCClient::handleQuit;
	// TODO :commandHandlers["NOTICE"] = &IRCClient::handleNotice;
	// TODO :commandHandlers["TOPIC"] = &IRCClient::handleNotice;
	// TODO :commandHandlers["KICK"] = &IRCClient::handleKick;
	// TODO :commandHandlers["LIST"] = &IRCClient::handleList;
	// TODO :commandHandlers["WHOIS"] = &IRCClient::handleWhois;
	// TODO :commandHandlers["INVITE"] = &IRCClient::handleInvite;
	// TODO :commandHandlers["NAMES"] = &IRCClient::handleNames;

	// ? Server Commands ?
	// TODO :commandHandlers["PASS"] = &IRCClient::handlePass;
	// TODO :commandHandlers["SERVER"] = &IRCClient::handleServer;
	// TODO :commandHandlers["CONNECT"] = &IRCClient::handleConnect;
	// Add more handlers if needed
}

// This function processes the "NICK" command and sets the nickname
void IRCClient::handleNick(const std::string& params) {
	std::string cleanNick = params;
	// Check and strip newline or any trailing whitespace characters if necessary
	if (!cleanNick.empty() && (cleanNick[cleanNick.length() - 1] == '\n' || cleanNick[cleanNick.length() - 1] == '\r')) {
		cleanNick.erase(cleanNick.find_last_not_of(" \n\r\t") + 1); // Trim right
	}
	setNick(cleanNick);
	std::cout << "Nickname set to: " << this->nick << std::endl;
}

// Getter for nickname
std::string IRCClient::getNickValue() {
	return this->nick;
}

void IRCClient::handleUser(const std::string& params) {
    // Example format: "username mode unused :realname"
    std::istringstream iss(params);
    std::string username, mode, unused, realname;

    // Read the username, mode, and unused (servername) directly
    if (!(iss >> username >> mode >> unused)) {
        std::cerr << "Invalid USER command format." << std::endl;
        return;
    }

    // The rest should be the real name, which might include spaces
    // Attempt to capture the entire line after ':'
    getline(iss, realname);
    size_t colonPos = realname.find(':');
    if (colonPos != std::string::npos) {
        realname = realname.substr(colonPos + 1);  // Extract real name after ':'
    } else {
        realname = "Anonymous";  // Default real name if not provided
    }

    setUserDetails(username, mode, unused, realname);
    std::cout << "User registered with username: " << username 
              << ", mode: " << mode << ", servername: " << unused 
              << " and real name: " << (realname.empty() ? "Anonymous" : realname) << std::endl;
}

void IRCClient::setUserDetails(const std::string& username, const std::string& mode, const std::string& servername, const std::string& realname) {
    this->user = username;
    this->mode = mode;  // Ensure there is a `mode` member variable in IRCClient
    this->host = servername;  // Ensure there is a `servername` member variable in IRCClient
    this->realname = realname;  // Ensure there is a `realName` member variable in IRCClient
}

void IRCClient::joinChannel(const std::string& channelName, const std::string& password) {
    // Request the server to join the channel
    bool success = server->joinChannel(channelName, this->nick, password);
    if (success) {
        std::cout << "Joined channel: " << channelName << std::endl;
    } else {
        std::cerr << "Failed to join channel: " << channelName << std::endl;
    }
}

void IRCClient::handleJoin(const std::string& params) {
    std::istringstream iss(params);
    std::string channelName, password;

    // Read the channel name and optionally a password
    iss >> channelName;
    getline(iss, password);  // Assume password may follow after a space

    if (channelName.empty() || channelName[0] != '#') {
        std::cerr << "Invalid channel name: " << channelName << std::endl;
        return;
    }

    // Trim potential leading whitespace in password
    password.erase(0, password.find_first_not_of(" \t"));

    // Call joinChannel to attempt to join the channel, with explicit instance qualification
    this->joinChannel(channelName, password);
}


// Function to execute commands based on the buffer received
bool IRCClient::runCommand(const std::string& buffer) {
	std::string command;
	std::string params;

	// Find the first space to separate command from parameters
	size_t spacePos = buffer.find(' ');
	if (spacePos != std::string::npos) {
		command = buffer.substr(0, spacePos);
		params = buffer.substr(spacePos + 1);
	} else {
		command = buffer;  // The command might not have parameters
	}

	// Find the command handler in the map
	std::map<std::string, CommandHandler>::iterator it = commandHandlers.find(command);
	if (it != commandHandlers.end()) {
		CommandHandler handler = it->second;
		(this->*handler)(params);  // Call the handler function
		return true;
	}

	std::cout << "Command not recognized: " << command << std::endl;
	return false;
}

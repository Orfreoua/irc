#include "headers.hpp"

void IRCClient::setUserDetails(const std::string &username,
							   const std::string &mode,
							   const std::string &servername,
							   const std::string &realname) {
  this->user = username;
  this->isUserSet = true;
  this->mode = mode; // Ensure there is a `mode` member variable in IRCClient
  this->host = servername; // Ensure there is a `servername` member variable in IRCClient
  this->realname = realname; // Ensure there is a `realName` member variable in IRCClient
}

void IRCClient::handleUser(const std::string &params) {
	// Example format: "username mode unused :realname"
	std::istringstream iss(params);
	std::string username, mode, unused, realname;

	// Check if the user is already registered
	if (isUserSet) {
		std::vector<std::string> emptyParams;
		server->sendFormattedReply(socket, ERR_ALREADYREGISTERED, emptyParams, "You may not reregister");
		return;
	}
	// Read the username, mode, and unused (servername) directly
	if (!(iss >> username >> mode >> unused)) {
		std::vector<std::string> emptyParams;
		server->sendFormattedReply(socket, ERR_NEEDMOREPARAMS, emptyParams, "Not enough parameters");
		return;
	}
	// The rest should be the real name, which might include spaces
	// Attempt to capture the entire line after ':'
	getline(iss, realname);
	size_t colonPos = realname.find(':');
	if (colonPos != std::string::npos) {
		realname = realname.substr(colonPos + 1); // Extract real name after ':'
	} else {
		realname = "Anonymous"; // Default real name if not provided
	}
	// Check for extra parameters beyond the expected ones
	std::string extra;
	if (iss >> extra) {
		std::cerr << "Too many parameters for USER command." << std::endl;
		std::vector<std::string> errorMsgParams;
		errorMsgParams.push_back("USER");
		this->server->sendFormattedReply(this->socket, ERR_UNKNOWNCOMMAND, errorMsgParams, "Too many parameters");
		return;
	}
	// Validate username (add your own rules for validation)
	if (username.empty() || mode.empty() || unused.empty() || realname.empty()) {
		std::vector<std::string> emptyParams;
		server->sendFormattedReply(socket, ERR_NEEDMOREPARAMS, emptyParams, "Not enough parameters");
		return;
	}
	setUserDetails(username, mode, unused, realname);
	isUserSet = true;
	std::cout << "User registered with username: " << username
			  << ", mode: " << mode << ", servername: " << unused
			  << " and real name: " << realname << std::endl;
	// Check if registration is successful
	registerAndWelcome(socket, nick);
}

bool IRCClient::isUserInUse(const std::string& user) {
	std::map<int, IRCClient*>::const_iterator it;
	const std::map<int, IRCClient*>& clients = server->getClients(); // Use getter
	for (it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getUserValue() == user) {
			return true;
		}
	}
	return false;
}

std::string IRCClient::getUserValue() { return this->user; }
#include "headers.hpp"


void	IRCClient::setNick(const std::string& nick)
{
	std::vector<std::string>::iterator it;

	for (it = channels.begin(); it != channels.end(); ++it)
	{
		std::string& str = *it;
		std::stringstream	ss;
		ss << server->getChan(str)->getRGB() << "[" << str << "]" << ": " << NORMAL << USER << getNick() << NORMAL << " changed his nickname to " << USER << nick << NORMAL << "\n";
		server->getChan(str)->msg(ss.str());
        server->getChan(str)->msg_n(getNick(), nick);
        server->getChan(str)->Rights.changeClientName(getNick(), nick);
		server->getChan(str)->rmUser(getNick());
		server->getChan(str)->addUser(getSocket(), nick);
        server->getChan(str)->Rights.changeClientName(getNick(), nick);
	}
	IRCClient* valueToMove = server->getUser(getNick());
    std::string to_send = getFullPrefix() + " NICK " + nick + "\r\n";
	msg(to_send);
    server->rmUser(getNick());
	server->addUser(nick, valueToMove);
	this->nick = nick;
    this->isNicked = true;
}


bool IRCClient::isNickValid(const std::string& nick) {
	// Check length: typically between 1 and 9 characters
	if (nick.length() < 1 || nick.length() > 9) {
		return false;
	}
	// Check first character: must be a letter or one of the special characters
	char firstChar = nick[0];
	if (!(std::isalpha(firstChar) || firstChar == '[' || firstChar == ']' || firstChar == '\\' ||
		  firstChar == '^' || firstChar == '_' || firstChar == '{' || firstChar == '|' || firstChar == '}')) {
		return false;
	}
	// Check remaining characters: must be letters, digits, or special characters
	for (std::string::size_type i = 1; i < nick.length(); ++i) {
		char c = nick[i];
		if (!(std::isalnum(c) || c == '-' || c == '[' || c == ']' || c == '\\' ||
			  c == '^' || c == '_' || c == '{' || c == '|' || c == '}')) {
			return false;
		}
	}
	return true;
}

// Function to check if the nickname is already in use (using the server's client list)
bool IRCClient::isNickInUse(const std::string& nick) {
    std::map<int, IRCClient*>::const_iterator it;
    const std::map<int, IRCClient*>& clients = server->getClients(); // Use getter
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickValue() == nick) {
            return true;
        }
    }
    return false;
}

void IRCClient::handleNick(const std::string &params) {
    std::string cleanNick = params;

    // Check and strip newline or any trailing whitespace characters if necessary
    if (!cleanNick.empty() && (cleanNick[cleanNick.length() - 1] == '\n' || cleanNick[cleanNick.length() - 1] == '\r')) {
        cleanNick.erase(cleanNick.find_last_not_of(" \n\r\t") + 1); // Trim right
    }
    // Extract the nickname from the parameters
    std::istringstream iss(cleanNick);
    std::string nick;
    if (!(iss >> nick)) {
        std::vector<std::string> emptyParams;
        server->sendFormattedReply(socket, ERR_NONICKNAMEGIVEN, emptyParams, "No nickname given");
        return;
    }
    // Check for extra parameters beyond the expected one
    std::string extra;
    if (iss >> extra) {
        std::cerr << "Too many parameters for NICK command." << std::endl;
        std::vector<std::string> errorMsgParams;
        errorMsgParams.push_back("NICK");
        this->server->sendFormattedReply(this->socket, ERR_UNKNOWNCOMMAND, errorMsgParams, "Too many parameters");
        return;
    }
    // Check if the nickname is valid
    if (!isNickValid(nick)) {
        std::vector<std::string> nickParams;
        nickParams.push_back("NICK");
        server->sendFormattedReply(socket, ERR_ERRONEOUSNICKNAME, nickParams, "Erroneous nickname");
        return;
    }
    // Check if the nickname is already in use
    if (isNickInUse(nick)) {
        std::vector<std::string> nickParams;
        nickParams.push_back("NICK");
        server->sendFormattedReply(socket, ERR_NICKNAMEINUSE, nickParams, "Nickname is already in use");
        return;
    }
    // Set the nickname
    setNick(nick);
    std::cout << "Nickname set to: " << this->nick << std::endl;
    registerAndWelcome(socket, nick);
}


// Getter for nickname
std::string IRCClient::getNickValue() { return this->nick; }


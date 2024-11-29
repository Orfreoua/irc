#include "headers.hpp"


IRCClient::IRCClient(int sock) 
	: socket(sock), realname(""), host(""), mode(""), isAuthenticated(false), isOperator(false), isNicked(false),
	  isUserSet(false), isRegistered(false)  {
	initCommandHandlers();
	
		std::stringstream ss;
		ss << sock;
		nick = "defaultUser_" + ss.str();
		user = "defaultUser_" + ss.str();

}

IRCClient::~IRCClient()
{
}

// Parameterized constructor with socket and host
IRCClient::IRCClient(int sock, const std::string &host)
	: socket(sock), nick(""), user(""), realname(""), host(host), mode(""),
	  isAuthenticated(false), isOperator(false), isNicked(false),
	  isUserSet(false), isRegistered(false) {
	
	initCommandHandlers(); // Initialize command handlers

	std::stringstream ss;
	ss << sock;
	nick = "defaultUser_" + ss.str();
	user = "defaultUser_" + ss.str();
}

void IRCClient::setServer(IRCServer *server) { this->server = server; }

int	IRCClient::getSocket() { return (socket); }

std::string	IRCClient::getNick()
{
	return this->nick;
}


void IRCClient::addChannel(const std::string& channelName)
{
	bool found = false;
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i] == channelName) {
			found = true;
			break;
		}
	}
	if (!found)
	{
	channels.push_back(channelName);
	}
}


void    IRCClient::msg(std::string str)
{
	const char* msg = str.c_str();
	ssize_t msg_len = std::strlen(msg);
	if (msg_len > 510)
	{
		str[510] = '\r';
		str[511] = '\n';
	}
	msg = str.c_str();
	send(getSocket(), msg, msg_len, 0);
}

void	IRCClient::rmChan(std::string chan)
{
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (*it == chan) {
            channels.erase(it);
            break; // Exit the loop once the element is found and removed
        }
    }
}

void IRCClient::initCommandHandlers() {
	commandHandlers["NICK"] = &IRCClient::handleNick;
	commandHandlers["USER"] = &IRCClient::handleUser;
	commandHandlers["JOIN"] = &IRCClient::handleJoin;
	commandHandlers["TOPIC"] = &IRCClient::handleTopic;
	commandHandlers["KICK"] = &IRCClient::handleKick;
	//commandHandlers["SET"] = &IRCClient::handleSet;
	commandHandlers["MODE"] = &IRCClient::handleMode;
	commandHandlers["PRIVMSG"] = &IRCClient::handlePrivMsg;
	commandHandlers["INVITE"] = &IRCClient::handleInvite;
	commandHandlers["PASS"] = &IRCClient::handlePass;
	commandHandlers["CAP"] = &IRCClient::handleCap;
	commandHandlers["PING"] = &IRCClient::handlePing;
	commandHandlers["MODE"] = &IRCClient::handleMode;
	commandHandlers["QUIT"] = &IRCClient::handleQuit;
}

const std::map<int, IRCClient*>& IRCServer::getClients() const {
	return clients;
}

// TODO : Not used, to integrate
bool IRCClient::isRegistrationSuccessful() const { return isNicked && isUserSet; }

std::vector<std::pair<std::string, std::string> > IRCClient::parseCommands(const std::string &buffer) {
	std::vector<std::pair<std::string, std::string> > commands;
	clientBuffer += buffer;

	size_t pos;
	while ((pos = clientBuffer.find("\r\n")) != std::string::npos) {
		std::string commandLine = clientBuffer.substr(0, pos);
		clientBuffer.erase(0, pos + 2);

		std::string command;
		std::string params;
		size_t spacePos = commandLine.find(' ');

		if (spacePos != std::string::npos) {
			command = commandLine.substr(0, spacePos);
			params = commandLine.substr(spacePos + 1);
		} else {
			command = commandLine; // The command might not have parameters
		}

		// Check and strip newline or any trailing whitespace characters from command
		if (!command.empty()) {
			command.erase(command.find_last_not_of(" \n\r\t") + 1); // Trim right
		}

		// Check and strip newline or any trailing whitespace characters from params
		if (!params.empty()) {
			params.erase(params.find_last_not_of(" \n\r\t") + 1); // Trim right
		}

		commands.push_back(std::make_pair(command, params));
	}
	return commands;
}

bool IRCClient::executeCommand(const std::string &command, const std::string &params) {
	// Check if the PASS command is required and has not been sent yet
  //  std::cout << "auth=" << isAuthenticated << " isnick=" << isNicked << " isuserset=" << isUserSet << std::endl;
	if (server->isPasswordRequired() && !isAuthenticated) {
		if (command != "PASS") {
			std::vector<std::string> errorMsgParams;
			errorMsgParams.push_back(command);
			server->sendFormattedReply(socket, ERR_NOTREGISTERED, errorMsgParams, "Password required");
			return false;
		}
	} else if (!isAuthenticated || !isNicked || !isUserSet) {
		if (command != "NICK" && command != "USER") {
			std::vector<std::string> errorMsgParams;
			errorMsgParams.push_back(command);
			server->sendFormattedReply(socket, ERR_NOTREGISTERED, errorMsgParams, "You have not registered");
			return false;
		}
	}

	std::map<std::string, CommandHandler>::iterator it = commandHandlers.find(command);
	if (it != commandHandlers.end()) {
		CommandHandler handler = it->second;
		(this->*handler)(params); // Call the handler function
		return true;
	} else {
		std::vector<std::string> errorMsgParams;
		errorMsgParams.push_back(command);
		server->sendFormattedReply(socket, ERR_UNKNOWNCOMMAND, errorMsgParams, "Unknown command");
		return false;
	}
}
std::string IRCClient::getFullPrefix() const {
  std::ostringstream ss;
  ss << ':' << nick << '!' << user << '@' << host;
  return ss.str();
}

bool IRCClient::getChan(std::string chan)
{
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
		// Si la cible est trouvée, retourner true
		if (*it == chan) return true;
	}
	// Si la cible n'est pas trouvée après avoir parcouru tout le vecteur
	return false;
}


void IRCClient::handlePing(const std::string &params) {
	if (params.empty()) {
		std::vector<std::string> errorMsgParams;
		errorMsgParams.push_back("PING");
		this->server->sendFormattedReply(this->socket, ERR_NEEDMOREPARAMS, errorMsgParams, "Not enough parameters"); // ERR_NEEDMOREPARAMS
		return;
	}

	std::string token = params;
	if (token[0] == ':') {
		token = token.substr(1);
	}

	if (token.empty()) {
		std::vector<std::string> errorMsgParams;
		errorMsgParams.push_back("PING");
		this->server->sendFormattedReply(this->socket, ERR_NOORIGIN, errorMsgParams, "No origin specified"); // ERR_NOORIGIN
		return;
	}

	// Generate a random token composed of only digits using Sodium library
	std::ostringstream oss;
	for (int i = 0; i < 10; ++i) {  // Generate a token of 10 digits
		oss << (randombytes_uniform(10));  // Generate a random digit between 0 and 9
	}
	std::string randomToken = oss.str();

	// Send PONG response with the generated random token
	std::string response = "PONG :" + randomToken + "\r\n";
	send(this->socket, response.c_str(), response.size(), 0);
}

void IRCClient::handleQuit(const std::string &params) {
    std::string quitMessage = "Client Quit";

    // Vérifier et extraire la raison si présente
    if (!params.empty()) {
        std::istringstream iss(params);
        std::string colonPrefix;
        std::getline(iss, colonPrefix, ':');
        std::getline(iss, quitMessage);
        if (quitMessage.empty()) {
            quitMessage = "Client Quit";
        }
    }

    // Envoyer le message de déconnexion à tous les canaux où le client est présent
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        std::string& str = *it;
        std::stringstream ss;
        ss << server->getChan(str)->getRGB() << "[" << str << "]" << ": " << NORMAL << USER << getNick() << NORMAL << " left" << "\n";
        server->getChan(str)->msg(ss.str());
        server->getChan(str)->rmUser(getNick());
    }

    // Journaliser l'événement de déconnexion
    std::cout << "Client " << getFullPrefix() << " has quit: " << quitMessage << std::endl;
    
    // Préparer le message de déconnexion à envoyer aux autres clients
    std::string to_send = getFullPrefix() + " QUIT";
    if (!quitMessage.empty()) {
        to_send += " :" + quitMessage;
    }
    to_send += "\r\n";
    msg(to_send);

    // Désactiver les événements de lecture/écriture pour ce descripteur de fichier
    server->disableReadEvents(this->socket);

    // Nettoyer et fermer la connexion
    server->deleteClient(this->socket);
}



void IRCClient::registerAndWelcome(int socket, std::string nick) {
	if (isRegistrationSuccessful()) {
	//	std::string clear_screen_command = "\033[2J\033[H";
	//	send(socket, clear_screen_command.c_str(), clear_screen_command.size(), 0);
		std::string welcomeArt = "\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠟⢛⣛⣛⣛⣛⡛⠿⢿⣿⣿⣿⡿⠿⠿⠿⠿⠿⢿⣿⣿⣿⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⢿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠋⣁⣴⣶⣿⣿⣿⣿⣿⣿⣿⣷⣶⠌⠉⣉⣤⣶⣶⣶⣶⣶⣤⣈⠙⢿⣿⣿\n\
                        ⣿⣿⣿⣿⣿⠿⠛⢉⣡⣤⣶⣶⣶⣦⣤⣬⣉⣉⠉⠤⢤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⣠⣾⠟⠉⠀⢀⣶⡄⠀⠙⢿⣷⣄⠹⣿\n\
                        ⣿⣿⡿⠋⣡⣴⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⢉⣀⣤⣤⣤⣤⣈⣉⠛⠿⣿⣿⣿⣿⣿⣿⣿⠃⣰⣿⠃⠀⠀⠀⣾⣿⠃⠀⣸⠀⠹⣿⡆⠹\n\
                        ⠟⢁⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⠟⢁⣤⣾⠟⠛⠉⠉⣩⣉⠉⠻⢷⣦⡈⢻⣿⣿⡿⠿⠋⠀⣿⡇⠀⠀⠀⣼⣿⠇⠀⣴⠃⠀⠀⢹⣿⠀\n\
                        ⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠋⣠⣿⡟⠁⠀⠀⠀⣸⣿⠏⠀⣰⠀⠻⣿⡄⠉⣡⡤⠶⠦⠀⣿⡇⠀⠀⣼⣿⡟⠀⣼⠃⠀⠀⠀⢸⣿⠀\n\
                        ⠿⠟⠋⠉⠉⠉⠉⠉⠛⠉⠁⢰⣿⡟⠀⠀⠀⠀⣴⣿⡟⠀⣰⠃⠀⠀⢹⣿⠀⣠⣤⣴⣦⡄⢻⣷⠀⢠⣿⡟⠀⣼⠃⠀⠀⠀⢀⣾⡟⠀\n\
                        ⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⠂⣾⣿⡇⠀⠀⠀⣸⣿⡟⠀⢠⡏⠀⠀⠀⢸⣿⠆⠻⣿⣿⣿⣿⡄⠻⣷⣌⠛⠀⣰⠏⠀⠀⠀⣠⣾⡿⠃⣠\n\
                        ⣤⣤⣄⣈⣉⠉⠛⠛⠻⢿⡆⢹⣿⡇⠀⠀⣰⣿⠏⠀⢀⡞⠀⠀⠀⠀⢸⡿⠀⢸⣿⣿⣿⣿⣿⣦⡉⠻⢷⣦⣤⣤⣤⣴⣿⠿⠋⣀⣾⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣷⣶⣤⣁⠘⣿⣷⡀⠰⣿⠏⠀⢀⡞⠀⠀⠀⠀⣠⡿⠃⢀⣾⣿⣿⣿⣿⣿⣿⣿⣷⣄⣀⣉⣉⣁⣀⣀⣰⢾⣿⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡈⠻⣿⣦⣈⠀⠀⠚⠁⠀⢀⣠⡾⠟⢀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠠⠸⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣄⠉⠛⠛⠷⠶⠖⠚⠛⠉⣀⣴⣿⣿⣿⣿⣿⠿⠛⠛⠋⠉⠉⠉⠉⣉⣉⣀⣀⣠⣤⣶⡄⠠⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣶⣶⣶⣾⣿⣿⣿⠟⠛⠋⠉⠀⠀⠀⣀⣀⣀⡀⠉⠙⠛⠛⠛⠛⠋⠉⠁⠀⢰⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⠉⠉⣁⣀⣠⣤⣤⣶⣾⣿⠟⠀⠀⢿⣿⣿⣿⣷⣦⡀⠐⢶⣾⣿⠿⠋⠀⣾⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⢰⣿⣿⣿⣿⣻⣿⣯⣽⣶⣿⠿⠟⠀⠀⣀⣀⠙⢿⣿⣿⡄⢠⣤⣤⣶⡆⣴⣿⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣤⣀⣈⡉⠉⠉⠉⣉⣉⣉⣀⣤⣴⣶⣶⣿⣿⣿⣇⠘⣿⣿⣇⠀⣿⣿⣿⠇⣿⢿⣿⣿\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠀⠻⠛⠛⠀⠈⣉⣀⣀⣤⡀⢨⣩\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠋⠀⣠⣤⣶⣶⣶⣾⣿⣿⣿⣿⣿⣿⣧⠀⢹\n\
                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠟⠛⠁⢠⣶⡄⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⢸\n\
                        ⠉⠛⠛⠛⠛⠛⠿⠿⠿⠿⠿⠿⠿⠿⠛⠛⠛⠛⠛⠛⠋⠉⠉⠉⠉⠀⠀⠀⠀⠀⠀⣼⣿⣿⣧⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠈\n\
                                  \n";
		std::cout << welcomeArt << std::endl << std::endl;
		send(socket, welcomeArt.c_str(), welcomeArt.size(), 0);
		std::vector<std::string> welcomeParams;
		welcomeParams.push_back(nick);
		std::string welcomeMessage = "Welcome to the IRC network " + getFullPrefix();
		server->sendFormattedReply(socket, RPL_WELCOME, welcomeParams, welcomeMessage);
		isRegistered = true;
	}
}
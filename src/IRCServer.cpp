#include "headers.hpp"

IRCServer& IRCServer::getInstance() {
		static IRCServer instance;
		return instance;
}

// IRCServer::IRCServer() : port(0), serverSocket(-1) {}

IRCServer::IRCServer() : port(0), hashedPass(""), serverSocket(-1), nfds(0) {
		for (int i = 0; i < MAX_CLIENTS + 1; ++i) {
				fds[i].fd = -1;  // Initialiser tous les descripteurs à -1
				fds[i].events = 0;
				fds[i].revents = 0;
		}
}

int IRCServer::getPort() const {
		return port;
}

IRCServer::~IRCServer() {
    for (std::map<std::string, Channel*>::iterator it = chan.begin(); it != chan.end(); ++it) {
        delete it->second; // delete the dynamically allocated Channel
    }
    chan.clear();
}

const IRCServerCapabilities &IRCServer::getCapabilities() const {
	return this
			->capabilities; // Return the reference to the capabilities instance
}

bool IRCServer::initSocket() {
	this->serverSocket = socket(AF_INET6, SOCK_STREAM, 0);
	if (this->serverSocket == -1)
		return (false);
	return (true);
}

void IRCServer::sendFormattedReply(int clientSocket, int replyCode,
																	 const std::vector<std::string> &params,
																	 const std::string &message) {
	std::string formattedMessage =
			MessageFormatter::formatIRCMessage(replyCode, params, message);
	std::stringstream ss;

	ss << getClient(clientSocket)->getFullPrefix() << formattedMessage << "\r\n";
	formattedMessage = ss.str();
	send(clientSocket, formattedMessage.c_str(), formattedMessage.length(), 0);
}

void IRCServer::setPass(const std::string &rawPass) {
	char hash[crypto_pwhash_STRBYTES];
	if (crypto_pwhash_str(hash, rawPass.c_str(), rawPass.length(),
												crypto_pwhash_OPSLIMIT_INTERACTIVE,
												crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
		throw std::runtime_error("Hashing pass failed: out of memory.");
	}
	this->hashedPass = std::string(hash);
}

bool IRCServer::verifyPass(const std::string &attempt, int clientSocket) {
	if (this->hashedPass.empty()) {
		std::cerr << "No hashed password set for verification." << std::endl;
		std::vector<std::string> params;
		params.push_back("Server"); // Manually push back the string into the vector
		sendFormattedReply(clientSocket, ERR_PASSWDMISMATCH, params,
											 "No password set on server.");
		return false;
	}

	if (crypto_pwhash_str_verify(this->hashedPass.c_str(), attempt.c_str(),
															 attempt.length()) != 0) {
		return false;
	}

	return true;
}

void IRCServer::setPort(int p) { this->port = p; }

bool IRCServer::bindSocket() {
	struct sockaddr_in6 serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin6_family = AF_INET6; // Utilise IPv6
	serverAddress.sin6_port =
			htons(this->port); // Port (en ordre d'octets en réseau)
	serverAddress.sin6_addr =
			in6addr_any; // Utilise toutes les interfaces disponibles
	if (bind(this->serverSocket, (struct sockaddr *)&serverAddress,
					 sizeof(serverAddress)) == -1) {
		close(this->serverSocket);
		return (false);
	}
	return (true);
}

bool IRCServer::isListening() {
	if (listen(this->serverSocket, 5) == -1) {
		close(this->serverSocket);
		return (false);
	}
	return (true);
}

void IRCServer::setupPolling() {
	fds[0].fd = this->serverSocket;
	fds[0].events = POLLIN | POLLOUT;
	nfds = 1; // Initial value 1 for server socket
}

bool IRCServer::handleNewConnection() {
	int clientSocket = accept(this->serverSocket, NULL, NULL);
	if (clientSocket == -1) {
		std::cerr << "Error code: " << errno << " (" << strerror(errno) << ")." << std::endl;
		close(this->serverSocket);
		return (false);
	}
	fds[nfds].fd = clientSocket;
	fds[nfds].events = POLLIN;
	nfds++;
	this->addClient(clientSocket);
	return (true);
}

// Handles reading data from the client and determining further action
void IRCServer::processClientData(int index) {
	char buffer[BUFFER_SIZE];
	ssize_t bytesRead = readClientData(fds[index].fd, buffer, BUFFER_SIZE);

	if (bytesRead <= 0) {
		handleDisconnection(index);
	} else {
		handleClientCommand(index, buffer);
	}
}

// Reads data from a client socket
ssize_t IRCServer::readClientData(int fd, char *buffer, int bufferSize) {
	ssize_t bytesRead = recv(fd, buffer, bufferSize - 1, 0);
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate the received data
	}
	return bytesRead;
}

// Handles the execution of a command from the buffer
void IRCServer::handleClientCommand(int index, const char *buffer) {
	(void)nfds;
		std::map<int, IRCClient *>::iterator clientIt = clients.find(fds[index].fd);
		if (clientIt != clients.end()) {
				std::vector<std::pair<std::string, std::string> > commandParamsList = clientIt->second->parseCommands(buffer);

				for (size_t i = 0; i < commandParamsList.size(); ++i) {
						const std::pair<std::string, std::string> &commandParams = commandParamsList[i];
						std::string command = commandParams.first;
						std::string params = commandParams.second;
						std::cout << "Processing command: " << command << " with params: " << params << std::endl;
						if (commandParams.first == "QUIT") {
							clientIt->second->handleQuit(commandParams.second);
						} else { 
								clientIt->second->executeCommand(command, params);
								std::string prefix = clientIt->second->getFullPrefix();
								std::cout << "Received message from " << (clientIt->second->isRegistered ? prefix : "unregistered client") << ": " << command << " " << params << std::endl;
						}
				}
		}
}

// Extracts the message content from the command parameters
std::string IRCServer::extractMessageContent(const std::string &params) {
	size_t firstSpace = params.find(' ');
	if (firstSpace != std::string::npos) {
		return params.substr(firstSpace + 1);
	}
	return "";
}

void IRCServer::broadcastMessage(int senderFd, const char *message) {
	// Find the sender's client object in the map of clients
	std::map<int, IRCClient *>::iterator it = clients.find(senderFd);
	if (it != clients.end()) {
		// Format the message by including the sender's nickname
		std::string formattedMessage = MessageFormatter::formatChatMessage(
				it->second->getNickValue(), message);
		const char *fullMessageCStr = formattedMessage.c_str();
		ssize_t fullMessageLength = std::strlen(fullMessageCStr);
		// Broadcast the formatted message to all connected clients except the
		// sender
		for (int i = 1; i < nfds; ++i) {
			if (fds[i].fd != senderFd &&
					fds[i].fd != -1) { // Ensure not sending back to the sender
				std::cout << ">> " << fullMessageCStr; // Logging the message to be sent
				ssize_t bytesSent =
						send(fds[i].fd, fullMessageCStr, fullMessageLength, 0);
				if (bytesSent == -1) {
					std::cerr << "Error sending message to client"
										<< std::endl; // Log error if send fails
				}
			}
		}
	}
}

void IRCServer::handleDisconnection(int index) {
	std::cout << "Client disconnected." << std::endl;
	close(fds[index].fd);

	this->deleteClient(fds[index].fd);
	for (int j = index; j < nfds - 1; ++j) {
		fds[j] = fds[j + 1];
	}
	nfds--;
}

bool IRCServer::acceptClients() {
		setupPolling(); // Mettre à jour cet appel pour ne pas passer les paramètres
		is_running = 1;
		while (is_running) {
				int pollResult = poll(fds, nfds, -1);
				if (pollResult == -1) {
						/*if (!is_running)
							std::cerr << "Error in poll" << std::endl;*/
						close(this->serverSocket);
						IRCServer::getInstance().closeAllClients();
						return (false);
				}
				if (fds[0].revents & POLLIN) {
						if (!handleNewConnection()) {
								return false;
						}
				}
				for (int i = 1; i < nfds; ++i) {
						if (fds[i].revents & POLLIN) {
								processClientData(i);
						}
				}
		}
		return (true); // This may be unreachable; consider how you want to handle termination.
}

void IRCServer::addClient(int clientSocket) {
		IRCClient* newClient = new IRCClient(clientSocket);  // Dynamically allocate a new IRCClient object
		newClient->server = this;
		this->clients[clientSocket] = newClient; // Store the pointer in the map
		addUser(newClient->nick, newClient);/////a ajouter
	std::cout << "New client connected " << " (socket:" << clientSocket << ")" << std::endl;
	return ;
}

void IRCServer::deleteClient(int clientSocket) {
		std::map<int, IRCClient*>::iterator it = clients.find(clientSocket);
		if (it != clients.end()) {
				delete it->second;  // Delete the dynamically allocated IRCClient
				clients.erase(it);  // Remove the entry from the map
		}
		close(clientSocket);
}

bool IRCServer::isPasswordRequired() const {
		return !hashedPass.empty();
}

void IRCServer::closeAllClients() {
		std::map<int, IRCClient *>::iterator it;
		for (it = clients.begin(); it != clients.end(); ++it) {
				close(it->first);
				delete it->second;
		}
		clients.clear();
}

void IRCServer::disableReadEvents(int clientSocket) {
		for (int i = 0; i < nfds; ++i) {
				if (fds[i].fd == clientSocket) {
						fds[i].events &= ~POLLIN; // Désactiver les événements de lecture
						// fds[i].events &= ~POLLOUT; // Désactiver les événements d'écriture si nécessaire
						break;
						std::cout << "Removed writing and reading" << std::endl;
				}
		}
}

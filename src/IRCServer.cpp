#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include "IRCClient.hpp"
#include "IRCServer.hpp"

IRCServer::IRCServer() {
    if (serverSocket != -1) { close(serverSocket); }
}

IRCServer::~IRCServer() {
    std::map<int, IRCClient*>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;  // Delete the dynamically allocated IRCClient
    }
    clients.clear();  // Clear the map after deleting all allocated objects
}

bool IRCServer::initSocket() {
	this->serverSocket = socket(AF_INET6, SOCK_STREAM, 0);
	if (this->serverSocket == -1) 
        return (false);
    return (true);
}

void IRCServer::setPassword(const std::string& pass) { this->password = pass; }

void IRCServer::setPort(int p) { this->port = p; }

bool IRCServer::bindSocket() {
 	struct sockaddr_in6 serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin6_family = AF_INET6; // Utilise IPv6
	serverAddress.sin6_port = htons(this->port); // Port (en ordre d'octets en réseau)
	serverAddress.sin6_addr = in6addr_any; //Utilise toutes les interfaces disponibles
	if (bind(this->serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		close(this->serverSocket);
		return (false);
	}
    return (true);

}

bool IRCServer::isListening() {
   if (listen(this->serverSocket, 5) == -1) 
   {
        close(this->serverSocket);
        return (false);
	}
    return (true);
    
}

void IRCServer::setupPolling(struct pollfd* fds, int& nfds) {
    fds[0].fd = this->serverSocket;
    fds[0].events = POLLIN;
    nfds = 1; // Initial value 1 for server socket
}

bool IRCServer::handleNewConnection(struct pollfd* fds, int& nfds) {
    int clientSocket = accept(this->serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        std::cerr << "Error: Failed to accept connection." << std::endl;
        close(this->serverSocket);
        return (false);
    }
    fds[nfds].fd = clientSocket;
    fds[nfds].events = POLLIN;
    nfds++;
    this->addClient(clientSocket);
    return (true);
}

void IRCServer::processClientData(struct pollfd* fds, int& nfds, int index) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = recv(fds[index].fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead <= 0) { handleDisconnection(fds, nfds, index); }
	else {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        // Assuming that the buffer might contain commands that need processing
        std::map<int, IRCClient*>::iterator it = clients.find(fds[index].fd);
        if (it != clients.end()) {
            it->second->runCommand(std::string(buffer));  // Process the command
        }
        // Broadcast the message to other clients
        broadcastMessage(fds, nfds, fds[index].fd, buffer);
    }
}

void IRCServer::broadcastMessage(struct pollfd fds[], int nfds, int senderFd, const char* message) {
    std::map<int, IRCClient*>::iterator it = clients.find(senderFd);
    if (it != clients.end()) {
        std::string formattedMessage = MessageFormatter::formatMessage(it->second->getNickValue(), message);
        const char* fullMessageCStr = formattedMessage.c_str();
        ssize_t fullMessageLength = std::strlen(fullMessageCStr);

        for (int i = 1; i < nfds; ++i) {
            if (fds[i].fd != senderFd) {
                std::cout << ">> " << fullMessageCStr;
                ssize_t bytesSent = send(fds[i].fd, fullMessageCStr, fullMessageLength, 0);
                if (bytesSent == -1)
                    std::cerr << "Error sending message to client" << std::endl;
            }
        }
    }
}

void IRCServer::handleDisconnection(struct pollfd* fds, int& nfds, int index) {
    std::cout << "Client disconnected." << std::endl;
    close(fds[index].fd);
    this->deleteClient(fds[index].fd);
    for (int j = index; j < nfds - 1; ++j) { fds[j] = fds[j + 1]; }
    nfds--;
}

bool IRCServer::acceptClients() {
    struct pollfd fds[MAX_CLIENTS + 1];
	int nfds = 0; // Declare nfds here, initializing to 0

    setupPolling(fds, nfds);
    while (true) {
        int pollResult = poll(fds, nfds, -1);
        if (pollResult == -1) {
            std::cerr << "Error in poll" << std::endl;
            close(this->serverSocket);
            return (false);
        }
        if (fds[0].revents & POLLIN) {
            if (!handleNewConnection(fds, nfds)) { return false; }
        }
        for (int i = 1; i < nfds; ++i) {
            if (fds[i].revents & POLLIN) { processClientData(fds, nfds, i); }
        }
    }
    return (true); // This may be unreachable; consider how you want to handle termination.
}

/*
-- Ajoute un client à la la liste des clients
*/
void IRCServer::addClient(int clientSocket) {
    IRCClient* newClient = new IRCClient();  // Dynamically allocate a new IRCClient object
	newClient->server = this;
    newClient->socket = clientSocket;        // Set the socket for this client
    this->clients[clientSocket] = newClient; // Store the pointer in the map
    std::cout << "New client connected: " << newClient->nick << " (socket:" << clientSocket << ")" << std::endl;
}

/*
-- Supprime un client à la la liste des clients
*/
void IRCServer::deleteClient(int clientSocket) {
    std::map<int, IRCClient*>::iterator it = clients.find(clientSocket);
    if (it != clients.end()) {
        delete it->second;  // Delete the dynamically allocated IRCClient
        clients.erase(it);  // Remove the entry from the map
    }
}

bool IRCServer::joinChannel(const std::string& channelName, const std::string& nick, const std::string& password) {
    std::map<std::string, ChannelInfo>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        // Channel exists, check conditions
        if (it->second.password != password) {
            std::cerr << "Incorrect password for channel: " << channelName << std::endl;
            return false;
        }
        
        std::vector<std::string>::iterator memberIt;
        bool isMember = false;
        for (memberIt = it->second.members.begin(); memberIt != it->second.members.end(); ++memberIt) {
            if (*memberIt == nick) {
                isMember = true;
                break;
            }
        }
        if (isMember) {
            std::cerr << "User already in channel: " << channelName << std::endl;
            return false;
        }

        // Add the user to the channel's members list
        it->second.members.push_back(nick);
        return true;
    } else {
        // Create the channel if it doesn't exist
        ChannelInfo newChannel(channelName, password);
        newChannel.members.push_back(nick);
        channels.insert(std::make_pair(channelName, newChannel));
        return true;
    }
}


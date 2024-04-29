#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

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
#include "ChannelInfo.hpp"

#define MAX_CLIENTS 1000 // Nombre maximal de clients pouvant être connectés
#define BUFFER_SIZE 1024 // Taille du buffer

class IRCClient; 

class MessageFormatter {
public:
    static std::string formatMessage(const std::string& nick, const std::string& message) {
        return nick + ": " + message;  // Basic format, can be expanded
    }
};

class IRCServer 
{
    private:
    	std::map<std::string, ChannelInfo> channels;  // Maps channel names to their info
		int port;
		std::string password;
		int serverSocket;
		int clientSockets[MAX_CLIENTS]; // Tableau pour stocker les descripteurs de socket des clients
		int numClients; // Nombre actuel de clients connectés
	
		std::map<int, IRCClient*> clients;

		void setupPolling(struct pollfd* fds, int& nfds);
		bool handleNewConnection(struct pollfd* fds, int& nfds);
		void processClientData(struct pollfd* fds, int& nfds, int index);
		void broadcastMessage(struct pollfd fds[], int nfds, int senderFd, const char* message);
		void handleDisconnection(struct pollfd* fds, int& nfds, int index);

    public:
		IRCServer();
		~IRCServer();

		bool initSocket();
		void setPassword(const std::string& pass);
		void setPort(int p);
		bool bindSocket();
		bool isListening();
		bool acceptClients();
		void addClient(int clientSocket);
		void deleteClient(int clientSocket);
		void createChannel(const std::string& name, const std::string& password);
    	bool joinChannel(const std::string& channelName, const std::string& nick, const std::string& password);
};

#endif 

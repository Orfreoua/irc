#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#include "headers.hpp"

#define MAX_CLIENTS 1000
#define BUFFER_SIZE 1024

class IRCClient;

class MessageFormatter {
public:
    static std::string formatChatMessage(const std::string& nick, const std::string& message) {
        return nick + ": " + message;
    }

    static std::string formatIRCMessage(int replyCode, const std::vector<std::string>& params, const std::string& defaultText) {
        std::ostringstream formattedMessage;
        formattedMessage << " " << std::setw(3) << std::setfill('0') << replyCode << " ";
        for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it) {
            formattedMessage << *it << " ";
        }
        formattedMessage << ":" << defaultText << "\r\n";
        return formattedMessage.str();
    }
};

class IRCServer {
private:
	IRCServer();
	IRCServer(const IRCServer&);
    IRCServer& operator=(const IRCServer&);
    IRCServerCapabilities capabilities;
    int			port;
    std::string	hashedPass;
    int			serverSocket;
    int			clientSockets[MAX_CLIENTS];
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds;
    std::map<int, IRCClient*> clients;
    std::map<std::string, IRCClient*> users;
    std::map<std::string, Channel*> chan;

    void		setupPolling();
    bool		handleNewConnection();
    void		processClientData(int index);
    ssize_t		readClientData(int fd, char* buffer, int bufferSize);
void    handleClientCommand(int index, const char *buffer);
    void		broadcastMessage(int senderFd, const char* message);
    std::string	extractMessageContent(const std::string& params);
    void		handleDisconnection(int index);

public:
    static IRCServer& getInstance();

    ~IRCServer();
	int getPort() const;

    bool	initSocket();
    void	sendFormattedReply(int clientSocket, int replyCode, const std::vector<std::string>& params, const std::string& message);
    void	setPass(const std::string& rawPass);
    bool	verifyPass(const std::string& attempt, int clientSocket);
    void	setPort(int p);
    bool	bindSocket();
    bool	isListening();
    bool	acceptClients();
	bool	callTestMdp(std::string chan_name, std::string pass);
    void	addClient(int clientSocket);
    void    closeAllClients();
    void	deleteClient(int clientSocket);
    void	createChannel(const std::string& name, const std::string& pass);
    bool	joinChannel(const std::string& channelName, const std::string& nick, const std::string& pass);

    const	IRCServerCapabilities& getCapabilities() const;
    const	std::map<int, IRCClient*>& getClients() const;
    bool	isPasswordRequired() const;

    Channel*	getChan(std::string channel_name);
    IRCClient*  getClient(int socket);
    IRCClient*	getUser(std::string user_name);
    void	addChan(std::string chan_name, std::string password);
    void	addUser(const std::string& nick, IRCClient* client);
    void	rmUser(std::string nick);
    void	msg(std::string msg);
    void	writeUsers();
    void	writeChans();
    void    disableReadEvents(int clientSocket);
};

#endif
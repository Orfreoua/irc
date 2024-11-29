#ifndef IRC_CLIENT_HPP
#define IRC_CLIENT_HPP

#include "headers.hpp"

class IRCClient {
public:
    int socket;
    	std::string clientBuffer;

    std::string nick;
    std::string user;
    std::string realname;
    std::string host;
    std::string mode;
    std::string passHash;
    bool isAuthenticated;
    bool isOperator;
    bool isNicked;
    bool isUserSet;
    bool isRegistered;

    std::vector<std::string> channels;
    typedef void (IRCClient::*CommandHandler)(const std::string&);
    std::map<std::string, CommandHandler> commandHandlers;
    IRCServer* server;

    IRCClient(int sock);
    IRCClient(int sock, const std::string& host);
    ~IRCClient();


    void setServer(IRCServer* server);
    bool runCommand(const std::string& buffer);
    void setNick(const std::string& nick);

    void handleNick(const std::string& params);
    void handleUser(const std::string& params);
    void handleJoin(const std::string& params);
    void handleKick(const std::string& params);
    void handleTopic(const std::string& params);
    void handlePrivMsg(const std::string& params);
    void handleInvite(const std::string& params);
    void handlePass(const std::string& params);
    void handleCap(const std::string& params);
    void handlePing(const std::string& params);
    void handleQuit(const std::string &params);
    void handleMode(const std::string &params);

    bool getChan(std::string chan);

    void addChannel(const std::string& channelName);
    void msg(std::string msg);
    void	rmChan(std::string chan);

    int getSocket();
    std::string getNick();
    std::string getUser();
    void setUser(const std::string& user);
    bool isUserInUse(const std::string& user);
    void setUserDetails(const std::string& username, const std::string& mode, const std::string& servername, const std::string& realname);
    std::string getNickValue();
    std::string getUserValue();
    bool isValidPass(const std::string& pass);
    void initCommandHandlers();
std::vector<std::pair<std::string, std::string> > parseCommands(const std::string &buffer);
    bool executeCommand(const std::string& command, const std::string& params);
    std::string getFullPrefix() const;
    bool isNickValid(const std::string& nick);
    bool isNickInUse(const std::string& nick);
    bool isRegistrationSuccessful() const;

    void handleSet(const std::string& params);
    bool isValidSetParams(std::string channel, std::string clientTarget, std::string right, std::string checkbox);
    void giveRight(std::string channel, std::string clientTarget, std::string right, std::string checkbox);
    void registerAndWelcome(int socket, std::string nick);
	//void		handleMode(const std::string &params);
	void		handleModeKey(std::istringstream &iss, const std::string &arg, const std::string &channel);
	void		handleModeOperator(std::istringstream &iss, const std::string &arg, const std::string &channel);
	void		handleModeLimit(std::istringstream &iss, const std::string &arg, const std::string &channel);
	void		handleModeTopic(std::istringstream &iss, const std::string &arg, const std::string &channel);
	void		handleModeInvite(std::istringstream &iss, const std::string &arg, const std::string &channel);


};

#endif

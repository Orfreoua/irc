#ifndef IRC_SERVER_CAPABILITIES_HPP
#define IRC_SERVER_CAPABILITIES_HPP

#include "headers.hpp"
#include <string>
#include <map>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define RPL_NICKCHANGE(old_nick, new_nick) (":" + std::string(old_nick) + " NICK " + std::string(new_nick) + "\r\n")
// Connection and general info replies
#define RPL_WELCOME            001
#define RPL_WELCOME_F(server_name, nick, user, host) (std::string(server_name) + " 001 " + std::string(nick) + " :Welcome to the Internet Relay Network " + std::string(nick) + "!" + std::string(user) + "@" + std::string(host) + "\r\n")


#define RPL_LEFTALLCHANNELS 900
#define RPL_LEFTALLCHANNELS_F(nick) (":" + std::string(nick) + " " + TOSTRING(RPL_LEFTALLCHANNELS) + " :You have left all channels.\r\n")

#define RPL_YOURHOST           002
#define RPL_CREATED            003

// User mode
#define RPL_UMODEIS            221

// Statistical replies
#define RPL_LUSERCLIENT        251
#define RPL_LUSERME            255

// Channel-specific replies
#define RPL_NOTOPIC_F(server_name, channel) (std::string(server_name) + " 331 " + std::string(channel) + " :No topic is set\r\n")
#define RPL_TOPIC_F(server_name, channel, topic) (std::string(server_name) + " 332 " + std::string(channel) + " " + std::string(topic) + "\r\n")
#define RPL_TOPIC              332
#define RPL_NAMREPLY           353
#define RPL_ENDOFNAMES         366

#define RPL_NAMREPLY 353
#define RPL_NAMREPLY_F(server_name, client, channel, nicks) (":" + std::string(server_name) + " " + "353" + " = " + std::string(client)+ " " + std::string(channel) + " :" + std::string(nicks) + "\r\n")

#define RPL_ENDOFNAMES 366
#define RPL_ENDOFNAMES_F(client, nick, channel) (":" + std::string(client) + " 366 " +std::string(nick) + " "+ std::string(channel) + " :End of /NAMES list\r\n")


// Server messages
#define RPL_MOTDSTART          375
#define RPL_ENDOFMOTD          376

#define RPL_INVITING(server_name, channel, nick) (std::string(server_name) + " 341 " + std::string(channel) + " " + std::string(nick) + "\r\n")
#define ERR_USERONCHANNEL(server_name, user, channel) (std::string(server_name) + " 443 " + std::string(user) + " " + std::string(channel) + " :is already on channel\r\n")

#define RPL_TOPICWHOTIME       333
#define RPL_TOPICWHOTIME_F(client, channel, nick, setat) (std::string(client) + " 333 " + std::string(channel) + " " + std::string(nick) + " " + std::string(setat) + "\r\n")

// Errors
#define ERR_NOSUCHNICK         401
#define ERR_NOSUCHNICK_F(server_name, nick) (std::string(server_name) + " 401 " + std::string(nick) + " :Nickname not found\r\n")

#define ERR_NOSUCHCHANNEL      403
#define ERR_NOSUCHCHANNEL_F(server_name, channel_name) (std::string(server_name) + " 403 * " + std::string(channel_name) + " :No such channel\r\n")

#define ERR_CANNOTSENDTOCHAN   404
#define ERR_CANNOTSENDTOCHAN_F(server_name, channel_name) (std::string(server_name) + " 404 * " + std::string(channel_name) + " :Cannot send to channel\r\n")

#define ERR_NOORIGIN           409
#define ERR_NORECIPIENT(command) (std::string("411 ") + "* " + std::string(command) + " :No recipient given (" + std::string(command) + ")\r\n")


#define ERR_UNKNOWNCOMMAND     421
#define ERR_UNKNOWNCOMMAND_F(server_name, nick, command) (std::string(server_name) + " 421 " + std::string(nick) + " " + std::string(command) + " :Unknown command\r\n")

#define ERR_NOTEXTTOSEND_F(server_name) (std::string(server_name) + " 412 * :No text to send\r\n")

#define ERR_NOMOTD             422
#define ERR_NONICKNAMEGIVEN    431
#define ERR_ERRONEOUSNICKNAME  432
#define ERR_NICKNAMEINUSE      433
#define ERR_USERNOTINCHANNEL_F(server_name, nick, channel) (std::string(server_name) + " 441 " + std::string(nick) + " " + std::string(channel) + " :They aren't on that channel\r\n")

#define ERR_NOTONCHANNEL       442
#define ERR_NOTONCHANNEL_F(server_name, nick, channel) (std::string(server_name) + " 442 " + std::string(nick) + " " + std::string(channel) + " :You're not on that channel\r\n")

#define ERR_NOTREGISTERED      451
#define ERR_NEEDMOREPARAMS     461
#define ERR_NEEDMOREPARAMS_F(server_name, command) (std::string(server_name) + " 461 * " + std::string(command) + " :Not enough parameters\r\n")

#define ERR_ALREADYREGISTERED  462
#define ERR_PASSWDMISMATCH     464
#define ERR_PASSWDMISMATCH_F(server_name) (std::string(server_name) + " 464 * :Password incorrect\r\n")

#define ERR_CHANNELISFULL_F(server_name, channel_name) (std::string(server_name) + " 471 * " + std::string(channel_name) + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN_F(server_name, channel) (std::string(server_name) + " 473 " + std::string(channel) + " :Cannot join channel (+i)\r\n")

#define ERR_BADCHANNELKEY_F(server_name, channel_name) (std::string(server_name) + " 475 * " + std::string(channel_name) + " :Cannot join channel (+k)\r\n")

#define ERR_CHANOPRIVSNEEDED   482
#define ERR_CHANOPRIVSNEEDED_F(server_name, nick, channel) (std::string(server_name) + " 482 " + std::string(nick) + " " + std::string(channel) + " :You're not channel operator\r\n")

#define ERR_UMODEUNKNOWNFLAG   501
#define ERR_UMODEUNKNOWNFLAG_F(server_name, nick) (std::string(server_name) + " 501 " + std::string(nick) + " :Unknown MODE flag\r\n")

//dernier ajout:

#define ERR_UNKNOWNMODE_F(server_name, user, badArg) (std::string(server_name) + " 472 " + std::string(user) + " " + std::string(badArg) + " :is unknown mode char to me\r\n")

#define RPL_INVITING_F(server_name, nick, channel) (":" + std::string(server_name) + " 341 " + std::string(nick) + " " + std::string(channel) + "\r\n")

#define RPL_CHANNELMODEIS_F(server_name, nick, channel_name, modes) (std::string(server_name) + " 324 " + std::string(nick) + " " + std::string(channel_name) + " " + std::string(modes) + "\r\n")

class IRCServerCapabilities {
private:
    std::map<std::string, std::string> capabilities;

public:
    IRCServerCapabilities();
    void add(const std::string& cap, const std::string& description);
    std::string formatForClient() const;
};

#endif
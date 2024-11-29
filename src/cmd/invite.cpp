#include "headers.hpp"

/*


Command: INVITE
   Parameters: <nickname> <channel>

   The INVITE command is used to invite a user to a channel.  The
   parameter <nickname> is the nickname of the person to be invited to
   the target channel <channel>.  There is no requirement that the
   channel the target user is being invited to must exist or be a valid
   channel.  However, if the channel exists, only members of the channel
   are allowed to invite other users.  When the channel has invite-only
   flag set, only channel operators may issue INVITE command.
 Top   ToC   RFC2812 - Page 22
   Only the user inviting and the user being invited will receive
   notification of the invitation.  Other channel members are not
   notified.  (This is unlike the MODE changes, and is occasionally the
   source of trouble for users.)

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_USERONCHANNEL
           ERR_CHANOPRIVSNEEDED
           RPL_INVITING                    RPL_AWAY


#include "headers.hpp"

*/
void IRCClient::handleInvite(const std::string& params)//message aux user du chan
{
    std::istringstream iss(params);
    std::string nick, chan, message;
	iss >> nick >> chan;
	
    size_t start = 0;
    while (start < message.size() && std::isspace(static_cast<unsigned char>(message[start]))) {
        ++start;
    }
    message.erase(0, start);
	std::getline(iss, message);  // Utiliser std::getline pour obtenir le reste de la ligne comme message
    if (nick.empty() || chan.empty())
    {//okokok
        //std::cout << "user = -" << user << "- " << std::endl;
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "Invalid format for INVITE params\r\n";
		server->msg(ss.str());
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "INVITE"));
		ss.str("");
		std::string syntaxMessage = "Usage: /INVITE <nick> <channel>\r\n";
    	msg(syntaxMessage);
        return ;
    }
    else if (!message.empty() || chan[0] != '#')
    {//okoko
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "Invalid format for INVITE params\r\n";
		server->msg(ss.str());
		msg(ERR_UNKNOWNCOMMAND_F(getFullPrefix(), getNick(), "INVITE"));
		ss.str("");
		std::string syntaxMessage = "Usage: /INVITE <nick> <channel>\r\n";
    	msg(syntaxMessage);
        return ;
    }
    else if (!getChan(chan))
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "INVITE when not on channel\r\n";
		server->msg(ss.str());
		msg(ERR_NOTONCHANNEL_F(getFullPrefix(), getNick(), chan));
		ss.str("");
		std::string syntaxMessage = "Usage: /INVITE <nick> <channel>\r\n";
    	msg(syntaxMessage);
        return ;
    }
    else if (!server->getUser(nick))
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "INVITE no such nick\r\n";
		server->msg(ss.str());
		msg(ERR_NOSUCHNICK_F(getFullPrefix(), nick));
        return ;
    }
    else if (server->getUser(nick) && server->getUser(nick)->getChan(chan))
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "INVITE already on channel\r\n";
		server->msg(ss.str());
		msg(ERR_USERONCHANNEL(getFullPrefix(), nick, chan));
        return ;
    }
    else if (server->getChan(chan)->getNbCli() >= server->getChan(chan)->mode.Vlimit)
    {
        std::stringstream ss;
		ss << "Channel is full\r\n";
		msg(ss.str());
        return ;
    }
    else if (server->getUser(nick) && server->getChan(chan) && server->getChan(chan)->Rights.getInvit(getNick()))
    {
            server->getUser(nick)->msg(RPL_INVITING_F(server->getUser(nick)->getFullPrefix(), nick, chan));
            server->getChan(chan)->addUser(server->getUser(nick)->getSocket(), nick);
			server->getUser(nick)->addChannel(chan);
			std::cout << nick << " entered " << chan << std::endl;
			//client
			std::string rgb = server->getChan(chan)->getRGB();
			std::string str = rgb + "[" + chan + "]" + ": " + NORMAL + USER + nick + NORMAL + " entered the chat\n";
			server->getChan(chan)->msg_from(str, nick);
			//msg(RPL_TOPIC_F(getFullPrefix(), "JOIN", server->getChan(chan)->getTopic()));
			//msg(RPL_TOPICWHOTIME_F(getFullPrefix(), chan, server->getChan(chan)->get_t_who(), server->getChan(chan)->get_t_time()));
			server->getUser(nick)->handleTopic(chan);
			server->getUser(nick)->msg(server->getChan(chan)->writeUsers());
			std::string to_send = server->getUser(nick)->getFullPrefix() + " JOIN " + chan + "\r\n";
			server->getChan(chan)->msg(to_send);
            server->getUser(nick)->msg(RPL_NAMREPLY_F(getFullPrefix(), nick, chan, server->getChan(chan)->writeUsers()));
			server->getUser(nick)->msg(RPL_ENDOFNAMES_F(getFullPrefix(), nick, chan));
            std::stringstream new_str;
            new_str << "You have been invited by " << USER << getNick() << NORMAL << " to join " << rgb << "[" << chan << "]" << NORMAL <<  " do a JOIN request to validate\n";
            server->getUser(nick)->msg(new_str.str());
    }
    else if (server->getUser(nick) && server->getChan(chan) && !server->getChan(chan)->Rights.getInvit(getNick()))
    {//okok
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "INVITE without the rights\r\n";
		server->msg(ss.str());
		msg(ERR_CHANOPRIVSNEEDED_F(getFullPrefix(), getNick(), chan));
        return ;
    }
}



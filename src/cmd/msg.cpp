#include "headers.hpp"


void IRCClient::handlePrivMsg(const std::string& params)//message aux user du chan
{
    std::istringstream iss(params);
    std::string user, message;
	iss >> user;
	

    std::getline(iss, message);  // Utiliser std::getline pour obtenir le reste de la ligne comme message
    if (user.empty())
    {
        msg(ERR_NORECIPIENT("PRIVMSG"));
        return ;
    }
    else if (user[0] != '#' && server->getUser(user) == NULL)
    {
      //  std::cout << "user = -" << user << "- " << std::endl;
        msg(ERR_NOSUCHNICK_F(getFullPrefix(), user));
        return ;
    }
    if (!message.empty() && message[0] == ' ')
    {
        message.erase(0, 1);
    }
	std::stringstream ss;

    if (user[0] == '#' && server->getChan(user) == NULL)
    {
        msg(ERR_NOSUCHCHANNEL_F(getFullPrefix(), user));
        return ;
    }
    else if (user[0] == '#' && !getChan(user))
    {
        msg(ERR_NOTONCHANNEL_F(getFullPrefix(), getNick(), user));
        return ;
    }
    else if (user[0] == '#' && server->getChan(user) && !message.empty() && message[0] == ':' && !getChan(user))
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "PRIVMSG to channel but user  not on channel\r\n";
		server->msg(ss.str());
		msg(ERR_NOTONCHANNEL_F(getFullPrefix(), getNick(), user));
    }
    else if (user[0] != '#' && server->getUser(user) == NULL && message[0] == ':')
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "PRIVMSG to user but user doesn't exist\r\n";
		server->msg(ss.str());
		msg(ERR_NOSUCHNICK_F(getFullPrefix(), user));
    }
    else if (message.empty())
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "empty message for PRIVMSG\r\n";
		server->msg(ss.str());
		msg(ERR_NOTEXTTOSEND_F(getFullPrefix()));
    }
    else if (server->getChan(user) && !server->getChan(user)->Rights.getWrite(getNick()))
    {
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << " don't have the writes\r\n";
		server->msg(ss.str());
		msg(ERR_CANNOTSENDTOCHAN_F(getFullPrefix(), user));///
    }
    else if (server->getUser(user) && user[0] != '#' && !message.empty() && message[0] == ':')
    {
        ss << getFullPrefix() << " PRIVMSG " << getNick() << " " << message << "\r\n";
        server->getUser(user)->msg(ss.str());
    }
    else if (user[0] == '#' && server->getChan(user) && !message.empty() && message[0] == ':' && getChan(user) && server->getChan(user)->Rights.getWrite(getNick()))
    {
        ss << getFullPrefix() << " PRIVMSG " << user << " " << message << "\r\n";
        server->getChan(user)->msg_from(ss.str(), nick);//message a tous les gens du chan sauf le nick client
    }
    else
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "Invalid format for PRIVMSG params\r\n";
		server->msg(ss.str());
		msg(ERR_UNKNOWNCOMMAND_F(getFullPrefix(), getNick(), "PRIVMSG"));
		ss.str("");
		std::string syntaxMessage = "Usage: /PRIVMSG <target> :<message>\r\n";
    	msg(syntaxMessage);
	}
}

#include "headers.hpp"
/*
/kick <channel> <nick> [<msg>]: (Operators only) Used to kick a user from a channel with an optional message. However, the command does not prohibit them from immediately rejoining the channel.
*/
//ne pas s'auto kick
void	IRCClient::handleKick(const std::string& params)
{
	std::istringstream iss(params);
    std::string chan, user, message;
	iss >> chan >> user;
	
	std::getline(iss, message);  // Utiliser std::getline pour obtenir le reste de la ligne comme message
   	size_t start = 0;
    while (start < message.size() && std::isspace(static_cast<unsigned char>(message[start]))) {
        ++start;
    }
    message.erase(0, start);
	if (chan.empty() || user.empty() || chan[0] != '#' || (!message.empty() && message[0] != ':'))
	{//okok

		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "Invalid format for KICK params\n";
		server->msg(ss.str());
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "KICK"));
		return ;
	}
	else if (!server->getChan(chan))//chan n'existe pas)
	{//okok

		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << getNick() << " tried to kick user " << USER << user << NORMAL << " from " << CHAN << chan << NORMAL << " but channel doesn't exist\n";
		server->msg(ss.str());
		msg(ERR_NOSUCHCHANNEL_F(getFullPrefix(), chan));
		return ;
	}
	else if (server->getChan(chan) && !getChan(chan))
	{//okok
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "KICK: can't kick because " << getNick() << " doesn't belong in this chan\r\n";
		server->msg(ss.str());
		msg(ERR_NOTONCHANNEL_F(getFullPrefix(), getNick(), chan));
		return ;
	}
	else if (!server->getChan(chan)->getUser(user))//user n'existe pas)
	{//pas sur
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << getNick() << " tried to kick unknown user " << USER << user << NORMAL << " from "  << server->getChan(chan)->getRGB() << chan << NORMAL << " \r\n";
		server->msg(ss.str());
		msg(ERR_USERNOTINCHANNEL_F(getFullPrefix(), user, chan));
		return ;
	}
	else if (!server->getChan(chan)->Rights.getQuick(getNick()))//n'a pas les droits)////////
	{//ok ok
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << getNick() << " tried to kick user " << USER << user << NORMAL << " from " << server->getChan(chan)->getRGB() << chan << NORMAL << " without the rights \n";
		server->msg(ss.str());
		ss.str("");
		ss << ERR_CHANOPRIVSNEEDED_F(getFullPrefix(), getNick(), chan);
		msg(ss.str());
		return ;
	}
    else if (message.empty())//Kick + pseudo  valide && user a les droits dans le chan + le user a kick existe)//il a les droits
	{//okok
		if (message[0] == ':')
			message.erase(0, 1);
		server->getChan(chan)->rmUser(user);
		std::stringstream	ss;
		ss << server->getChan(chan)->getRGB() << "[" << chan << "]" << ": " << NORMAL << USER << getNick() <<  NORMAL << " kicked " << USER << user << NORMAL << " from " << server->getChan(chan)->getRGB() << chan << NORMAL << " for no reason\n";
		server->msg(ss.str());//message de confirmation au server
		std::string to_send = getFullPrefix() + " KICK " + chan + " " + user + " " + message + "\r\n";
		server->getChan(chan)->msg(to_send);
		server->getUser(user)->msg(to_send);
		server->getUser(user)->rmChan(chan);
		return ;
	}
	else if (!message.empty())//Kick + pseudo  valide && user a les droits dans le chan + le user a kick existe)//il a les droits
	{//okoko
		if (message[0] == ':')
			message.erase(0, 1);
		server->getChan(chan)->rmUser(user);
		std::stringstream	ss;
		ss << server->getChan(chan)->getRGB() << "[" << chan << "]" << ": " << NORMAL << USER << getNick() <<  NORMAL << " kicked " << USER << user << NORMAL << " from " << server->getChan(chan)->getRGB() << chan << NORMAL << " using " << message << " as the reason\n";
		server->msg(ss.str());//message de confirmation au server
		std::string to_send = getFullPrefix() + " KICK " + chan + " " + user + " " + message + "\r\n";
		server->getChan(chan)->msg(to_send);
		server->getUser(user)->msg(to_send);
		server->getUser(user)->rmChan(chan);
		return ;
	}
	else
	{
        std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "KICK: Invalid format\n";
		server->msg(ss.str());
		ss.str("");
		ss << ERR_NEEDMOREPARAMS_F(getFullPrefix(), "KICK");
		msg(ss.str());
		ss.str(""); 
		std::string syntaxMessage = "Usage: /KICK <channel> <nick> :<reason>\r\n";
		msg(syntaxMessage);
    }
}
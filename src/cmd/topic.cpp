#include "headers.hpp"

/*std::string RPL_TOPICWHOTIME_F(const std::string& prefix, const std::string& channel, const std::string& who, const std::string& time) {
    return ":" + prefix + " 333 " + channel + " " + who + " " + time + "\r\n";
}

// Exemple de réponse RPL_TOPIC
std::string RPL_TOPIC_F(const std::string& prefix, const std::string& channel, const std::string& topic) {
    return ":" + prefix + " 332 " + channel + " :" + topic + "\r\n";
}*/

void	IRCClient::handleTopic(const std::string& param)
{
    std::string  cmd, chan, message;
	std::istringstream iss(param);
	iss >> chan;
	std::getline(iss, message);
	size_t start = 0;
	/*if (server->getChan(chan)->mode.topic == true)
		std::cout << REDLIGHT << "Error: " << NORMAL << "TRUEUEUEEU\n"<< std::endl;
	if (server->getChan(chan)->mode.topic == false)
		std::cout  << REDLIGHT << "Error: " << NORMAL << "NOOOOOOOOOOOO\n" << std::endl;
	if (server->getChan(chan)->Rights.getModes(getNick()) != false)
		std::cout  << REDLIGHT << "Error: " << NORMAL << "ELSEEE nooo\n" << std::endl;*/





    while (start < message.size() && std::isspace(static_cast<unsigned char>(message[start]))) {
        ++start;
    }
    message.erase(0, start);
	//std::cout << "channel name = -" << chan << "- MESSAGE de 0 ========= -" << message[0] << "- "<< message << std::endl;
    if (chan[0] != '#')
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "Invalid format for TOPIC params\n";
		server->msg(ss.str());
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "TOPIC"));
		return ;
	}
	else if (server->getChan(chan) == NULL)
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << getNick() << " failed to change unexisting channel topic\n";
		server->msg(ss.str());
		msg(ERR_NOSUCHCHANNEL_F(getFullPrefix(), chan));
		return ;
	}
	else if (std::find(channels.begin(), channels.end(), chan) == channels.end())
	{
		//parcours channels et print les elements
		msg(ERR_NOTONCHANNEL_F(getFullPrefix(), getNick(), chan));//tu ne fais pas partie du chan
		return ;
    }
	else if ((server->getChan(chan)->mode.topic == true) && server->getChan(chan) != NULL && message.empty())//print du topic
	{
		if (server->getChan(chan)->getTopic().empty())
			msg(RPL_NOTOPIC_F(getFullPrefix(), chan));
		else
		{
   			msg(RPL_TOPIC_F(getFullPrefix(), chan, server->getChan(chan)->getTopic()));			//who time?
		}
		return ;
	}
	else if (server->getChan(chan)->mode.topic == false && server->getChan(chan)->Rights.getModes(getNick()) && server->getChan(chan) != NULL && message.empty())//print du topic
	{
		if (server->getChan(chan)->getTopic().empty())
			msg(RPL_NOTOPIC_F(getFullPrefix(), chan));
		else
		{
   			msg(RPL_TOPIC_F(getFullPrefix(), chan, server->getChan(chan)->getTopic()));			//who time?
		}
		return ;
	}
	/*else if ((server->getChan(chan)->mode.topic == true || (server->getChan(chan)->mode.topic == false && server->getChan(chan)->Rights.getModes(getNick()) != false)) && server->getChan(chan) != NULL && message.empty())//print du topic
	{
		if (server->getChan(chan)->getTopic().empty())
			msg(RPL_NOTOPIC_F(getFullPrefix(), chan));
		else
		{
   			msg(RPL_TOPIC_F(getFullPrefix(), chan, server->getChan(chan)->getTopic()));			//who time?
		}
		return ;
	}*/
	else if ((server->getChan(chan)->mode.topic && !server->getChan(chan)->Rights.getModes(getNick())))
	{
		msg(ERR_CHANOPRIVSNEEDED_F(getFullPrefix(), getNick(), chan));
		return ;
	}
	else if (server->getChan(chan)->Rights.getModes(getNick()) && server->getChan(chan) != NULL && !message.empty() && message[0] == ':' )//&& server->getChan(chan)->mode.topic)//  && user est operator du chan)
	{
		message.erase(0,1);
		std::stringstream ss;
		ss << USER << getNick() << NORMAL << " changed channel " << server->getChan(chan)->getRGB() << chan << NORMAL << " topic\n";
		server->msg(ss.str());  // Message de confirmation au serveur
		ss.str("");
		ss << "You successfully changed " << server->getChan(chan)->getRGB() << chan << NORMAL << " topic\n";  // Message de confirmation à l'utilisateur
		msg(ss.str());
		ss.str("");
		ss << USER << getNick() << NORMAL << " changed TOPIC\n";
		server->getChan(chan)->msg(ss.str());  // Message d'annonce aux utilisateurs du canal
		server->getChan(chan)->setTopic(message);
		std::string to_send = getFullPrefix() + " TOPIC " + chan + " :" + message + "\r\n";
		if (server->getChan(chan)->mode.topic == true)//print du topic
		{
			if (server->getChan(chan)->getTopic().empty())
				server->getChan(chan)->msg(to_send);
			else
			{
				server->getChan(chan)->msg(to_send);
			}
			//return ;
		}
		if ((server->getChan(chan)->mode.topic == false && server->getChan(chan)->Rights.getModes(getNick())))//print du topic
		{
			if (server->getChan(chan)->getTopic().empty())
				server->getChan(chan)->msg_o(to_send);
			else
			{
				server->getChan(chan)->msg_o(to_send);
			}
			//return ;
		}
		msg(RPL_TOPICWHOTIME_F(getFullPrefix(), chan, server->getChan(chan)->get_t_who(), server->getChan(chan)->get_t_time()));
		msg(RPL_TOPIC_F(getFullPrefix(), chan, server->getChan(chan)->getTopic()));
    	return;
	}
	else if (!server->getChan(chan)->Rights.getModes(getNick()))// && user n'est pas operator du chan)
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << getNick() << " failed to change channel topic when not root\n";
		server->msg(ss.str());
		msg(ERR_CHANOPRIVSNEEDED_F(getFullPrefix(), getNick(), chan));
		return ;
	}
	else if (server->getChan(chan) != NULL && !message.empty() && message[0] == ':' && !server->getChan(chan)->mode.topic)//&& !server->getChan(chan).mode.topic)// && user n'est pas operator du chan)
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << getNick() << " failed to change channel topic when topic has been locked\n";
		server->msg(ss.str());
		ss.str("");
		ss << "You failed to change channel topic when topic has been locked\n";
		msg(ss.str());
		return ;
	}
	else
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "Invalid format for TOPIC params\n";
		server->msg(ss.str());
		msg(ERR_UNKNOWNCOMMAND_F(getFullPrefix(), getNick(), "TOPIC"));
		ss.str("");
		std::string syntaxMessage = "Usage: /TOPIC <channel> :<new_topic>\n";
    	msg(syntaxMessage);
	}
	return ;
}


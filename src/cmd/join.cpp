#include "headers.hpp"

/**
/list: Used to list all current channels, this command is not recommended (showing hundreds of channels may take minutes).
/join <channel>: This command will join an existing channel or create a new one with the given name. If you are on several channels at once, repeated join commands will let you change your current channel.
/names [<channel> | *]: This command displays a list of all users on the given (or current) channel. Using this command without parameters lists all users on IRC, and is not recommended.
/who [<channel> | *]: This command displays a list of all users, and certain user information, for the given (or current) channel. Using this command without parameters lists all users on IRC, and is not recommended.
/topic <channel> [<topic>]: To display the channel's topic, use the simple form. To change the channel's topic, enter a new one with the command.
*/


//condition mdp vide ou whitespace
void IRCClient::handleJoin(const std::string& params)
{
	std::istringstream iss(params);
	std::string chan_name, password, rest;

	iss >> chan_name >> password;
	getline(iss, rest);  // Assume password may follow after a space

	if (!rest.empty())
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "JOIN: Invalid channel name\n";
		server->msg(ss.str());
		ss.str("");
		ss << ERR_UNKNOWNCOMMAND_F(getFullPrefix(), getNick(), "JOIN + args");
		msg(ss.str());
		ss.str(""); 
		std::string syntaxMessage = "Usage: /JOIN <channel> <key>\n";
		msg(syntaxMessage);
		return ;
	}
	else  if (chan_name != "" && chan_name[0] == '#' && chan_name != "#" && server->getChan(chan_name) && !server->getChan(chan_name)->mode.invitOnly)//  server->chan.find(chan_name) != server->chan.end())//si channel existe et password est vide ou bon password
	{
		if (server->getChan(chan_name)->getNbCli() >= server->getChan(chan_name)->mode.Vlimit)
		{
			std::stringstream ss;
			ss << REDLIGHT << "Error: " << NORMAL << "JOIN: Channel is full\n";
			server->msg(ss.str());
			ss.str("");
			ss << ERR_CHANNELISFULL_F(getFullPrefix(), chan_name);
			msg(ss.str());
			ss.str(""); 
			std::string syntaxMessage = "Usage: /JOIN <channel>\n";
			msg(syntaxMessage);
		}
		else if ((server->getChan(chan_name)->mode.pass == false || (server->getChan(chan_name)->mode.pass == true && server->callTestMdp(chan_name, password))) && server->getChan(chan_name)->getNbCli() <= server->getChan(chan_name)->mode.Vlimit)
		{
			server->getChan(chan_name)->addUser(getSocket(), getNick());
			addChannel(chan_name);
			std::cout << nick << " entered " << chan_name << std::endl;
			//client
			std::string rgb = server->getChan(chan_name)->getRGB();
			std::string str = rgb + "[" + chan_name + "]" + ": " + NORMAL + USER + nick + NORMAL + " entered the chat\n";
			server->getChan(chan_name)->msg_from(str, getNick());
			//msg(RPL_TOPIC_F(getFullPrefix(), "JOIN", server->getChan(chan_name)->getTopic()));
			//msg(RPL_TOPICWHOTIME_F(getFullPrefix(), chan_name, server->getChan(chan_name)->get_t_who(), server->getChan(chan_name)->get_t_time()));
			std::string to_send = getFullPrefix() + " JOIN " + chan_name + "\r\n";
			server->getChan(chan_name)->msg(to_send);
		
			msg(RPL_NAMREPLY_F(getFullPrefix(), getNick(), chan_name, server->getChan(chan_name)->writeUsers()));
			msg(RPL_ENDOFNAMES_F(getFullPrefix(), getNick(), chan_name));
			//msg(server->getChan(chan_name)->writeUsers());

			handleTopic(chan_name);
		}
		else
		{
			std::cout << nick << " failed " << chan_name << " password" << std::endl;
			//client
			msg(ERR_BADCHANNELKEY_F(getFullPrefix(), chan_name));
		}
		//ERR_BADCHANNELKEY_F(server_name, channel_name)
	}
	else if (chan_name != "" && chan_name[0] == '#' && chan_name != "#" && chan_name != "#0" && server->getChan(chan_name) == false)//si channel N'existe PAS
	{
		server->addChan(chan_name, password);
			addChannel(chan_name);
		server->getChan(chan_name)->addUser(getSocket(), getNick());
///ici add les droits du operator
		server->getChan(chan_name)->Rights.newClient(nick, server->getChan(chan_name)->Rights.RootRights());
		//server->getChan(chan_name)->getTopic();
		std::string tmp = getNick() + " created channel " + chan_name; 
		std::string str = "You created " + chan_name;
		if (password != "")//il y a un pass
		{
			server->getChan(chan_name)->mode.pass = true;
			str = str + " with this password : " + password + "\n";
		}
		else
		{
			server->getChan(chan_name)->mode.pass = false;
			str = str + " with no password\n";
		}
		msg(str);//// a vérifier
		msg(RPL_NAMREPLY_F(getFullPrefix(), getNick(), chan_name, server->getChan(chan_name)->writeUsers()));
		msg(RPL_ENDOFNAMES_F(getFullPrefix(), getNick(), chan_name));
	}
	else if ((chan_name == "0"  || chan_name == "#0") && password.empty())
	{
		std::vector<std::string>::iterator it;
		for (it = channels.begin(); it != channels.end(); ++it)
		{
			std::string& str = *it;
			std::stringstream	ss;
			ss << server->getChan(str)->getRGB() << "[" << str << "]" << ": " << NORMAL << USER << getNick() << NORMAL << " left" << "\n";
			server->getChan(str)->msg(ss.str());
			server->getChan(str)->rmUser(getNick());
		}
	}
	else if (chan_name.empty())
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "JOIN: Invalid channel name\n";
		server->msg(ss.str());
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "JOIN"));
		std::string syntaxMessage = "Usage: /JOIN <channel> <key>\n";
		msg(syntaxMessage);
		return ;
	}
	else if (server->getChan(chan_name) && server->getChan(chan_name)->mode.invitOnly)
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "JOIN: Tried to join an invite only channel\n";
		server->msg(ss.str());
		msg(ERR_INVITEONLYCHAN_F(getFullPrefix(), chan_name));
	}
	else
	{
		std::stringstream ss;
		ss << REDLIGHT << "Error: " << NORMAL << "JOIN: Invalid channel name\n";
		server->msg(ss.str());
		ss.str("");
		ss << ERR_UNKNOWNCOMMAND_F(getFullPrefix(), getNick(), "JOIN");
		msg(ss.str());
		ss.str(""); 
		std::string syntaxMessage = "Usage: /JOIN <channel> <key>\n";
		msg(syntaxMessage);
	}
}

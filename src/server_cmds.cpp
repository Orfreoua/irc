#include "headers.hpp"

Channel* IRCServer::getChan(std::string channel_name)
{
	std::map<std::string, Channel*>::iterator it = chan.find(channel_name);

    if (it != chan.end())
        return it->second;
	else  
		return (NULL);
}

IRCClient*	IRCServer::getUser(std::string user_name)
{
	std::map<std::string, IRCClient*>::iterator it = users.find(user_name);

    if (it != users.end())
        return it->second;
	else
        return (NULL);
}

IRCClient*	IRCServer::getClient(int socket)
{
	std::map<int, IRCClient*>::iterator it = clients.find(socket);

    if (it != clients.end())
        return it->second;
	else
        return (NULL);
}

void IRCServer::addChan(std::string chan_name, std::string password)
{
    Channel* channel = new Channel(chan_name, password);
	this->chan[chan_name] = channel;
}

void IRCServer::addUser(const std::string& nick, IRCClient* client/*, std::map<std::string, IRCClient*>& user*/)
{
    users[nick] = client;

}

bool	IRCServer::callTestMdp(std::string chan_name, std::string pass)//pour acceder au channel privé pour tester le mdp du channel
{
	return (chan.find(chan_name)->second->testMdp(pass));
}

void IRCServer::msg(std::string msg)
{
	std::cout << msg << std::endl;
}

void IRCServer::writeUsers()
{
	std::map<std::string, IRCClient*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it) {
		//std::cout << " Nick: " << it->second->getNick() << "\n Socket: " << it->second->getSocket() << "\n Username: " << it->second->getUser()  << std::endl; 
        std::cout << "key de la map: -" <<  it->first << "-   ";
		std::cout << " Nick: " << it->second->getNick() << std::endl;
	}
}

void	IRCServer::rmUser(std::string nick)
{
	if (users.find(nick) != users.end())
		users.erase(users.find(nick));
}

void IRCServer::writeChans()
{
	std::map<std::string, Channel*>::iterator it;
	for (it = chan.begin(); it != chan.end(); ++it) {
		it->second->printChan(); 
	}
}
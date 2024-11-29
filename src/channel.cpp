#include "headers.hpp"

Channel::Channel(std::string chan_name, std::string password) : chan_name(chan_name), mdp(password), topic(""), t_time(""), t_who("")
{
	mode.invitOnly = false;
	mode.topic = true;
	mode.changeLimitUsers = false;
	mode.Vlimit = MAX_CLIENTS;
	this->maxClients = MAX_CLIENTS;
	setRGB(BASICCHAN);
}

Channel::~Channel()
{
/*=======================================================================================*/
	std::map<std::string, IRCClient*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
	{
		delete it->second;  // Delete the dynamically allocated IRCClient
	}
	users.clear();
/*=======================================================================================*/
}

void Channel::setMdp(const std::string& newMdp) {
        mdp = newMdp;
    }

std::string Channel::get_t_time()
{
	return (t_time);
}

std::string Channel::get_t_who()
{
	return (t_who);
}

std::string Channel::getCurrentTimestamp() {
    std::time_t currentTime = std::time(NULL);
    std::stringstream ss;
    ss << currentTime;
    return ss.str();
}

// Function to set the RPL_TOPICWHOTIME response
void Channel::setTopicWhoTime(std::string nick)
{
    t_time = getCurrentTimestamp();
	t_who = nick;
    return ;
}

void	Channel::setRGB(int rgb)
{
    if (rgb < 0 || rgb > 999999999)
	{
        std::cout << "Le nombre doit être compris entre 0 et 999999999 pour créer un RGB." << std::endl;/// message a revoir
        return;
    }
    r = (rgb / 1000000) % 1000;
    g = (rgb / 1000) % 1000;
    b = rgb % 1000;
}

std::string	Channel::getRGB()
{
	std::ostringstream ss_r, ss_g, ss_b;
    ss_r << r;
    ss_g << g;
    ss_b << b;

    std::string str = "\033[38;2;" + ss_r.str() + ";" + ss_g.str() + ";" + ss_b.str() + "m";
    return (str);
}

void Channel::addUser(int userSocket, std::string nick)
{
	IRCClient* newClient = new IRCClient(userSocket);
	newClient->nick = nick;
	this->users[nick] = newClient;//verifier si nick est deja pris
	this->Rights.newClient(nick, this->Rights.BasicRights());
}

void Channel::rmUser(std::string nick) {
    std::map<std::string, IRCClient*>::iterator it = users.find(nick);
    if (it != users.end()) {
        delete it->second;  // Delete the dynamically allocated IRCClient
        users.erase(it);
    }
}

std::string Channel::getChan()
{
	return (chan_name);
}

void Channel::printChan()
{
	std::cout << getChan() << std::endl;
}

std::string Channel::writeUsers() {
    std::map<std::string, IRCClient*>::iterator it;
	std::stringstream ss;


    for (it = --users.end(); it != users.begin(); --it)
	{
		if (Rights.getModes(it->second->getNick()) == true)//c'est un operateur
			ss << "@";
		ss << it->second->getNick();
		ss << ","; // nom
    }
	if (Rights.getModes(users.begin()->second->getNick())) // C'est un opérateur
    	ss << "@";
	ss << users.begin()->second->getNick();
	std::string str = ss.str();
	return(str);
}

void	Channel::msg(std::string	str)
{
	std::map<std::string, IRCClient*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
	{
		it->second->msg(str); 
	}
	return ;
}

void	Channel::msg_o(std::string	str)
{
	std::map<std::string, IRCClient*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
	{
		if (Rights.getModes(it->second->getNick()))
			it->second->msg(str); 
	}
	return ;
}

void	Channel::msg_n(std::string	old, std::string now)
{
	std::map<std::string, IRCClient*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
	{
		it->second->msg(RPL_NICKCHANGE(old, now)); 
	}
	return ;
}

void	Channel::msg_from(std::string	str, std::string nick)
{
	std::map<std::string, IRCClient*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
	{
		if (it->second->getNick() != nick)
			it->second->msg(str); 
	}
	return ;
}

bool    Channel::testMdp(std::string pass)
{
	return (pass == mdp) ? true : false;
}

bool    Channel::getUser(std::string user)
{
    std::map<std::string, IRCClient*>::iterator it = users.find(user);
    return (it != users.end());
}

std::string	Channel::getTopic()
{
	return (topic);
}

void	Channel::setTopic(std::string message)
{
	topic = message;
	return ;
}

int         Channel::getNbCli()
{
	return (users.size());
}
std::string    Channel::listModes(bool is_operator) {
    std::string list;

    list += (mode.invitOnly ? "+i, " : "-i, ");
    list += (mode.topic ? "+t, " : "-t, ");
    list += (mode.pass ? "+k, " : "-k, ");
    list += (is_operator ? "+o, " : "-o, ");
    list += (mode.invitOnly ? "+l." : "-l.");
    return list;
}

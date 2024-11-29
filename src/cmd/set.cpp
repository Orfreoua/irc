#include "headers.hpp"

void	IRCClient::giveRight(std::string channel, std::string clientTarget, std::string right, std::string checkbox){
	bool	checkboxValue = false;

	if (checkbox == "yes")
		checkboxValue = true;

	if (right == "write")
		server->getChan(channel)->Rights.setWrite(clientTarget, checkboxValue);
	else if (right == "quick")
		server->getChan(channel)->Rights.setQuick(clientTarget, checkboxValue);
	else if (right == "invit")
		server->getChan(channel)->Rights.setInvit(clientTarget, checkboxValue);
	else if (right == "modes")
		server->getChan(channel)->Rights.setModes(clientTarget, checkboxValue);
	else if (right == "give")
		server->getChan(channel)->Rights.setGive(clientTarget, checkboxValue);
}

bool	IRCClient::isValidSetParams(std::string channel, std::string clientTarget, std::string right, std::string checkbox) {
	if (!server->getChan(channel)) {
		msg(ERR_NOSUCHCHANNEL_F(getFullPrefix(), channel));
		return false;
	}
	if (!getChan(channel)) {
		msg(ERR_NOTONCHANNEL_F(getFullPrefix(), getNick(), channel));
		return false;
	}
	if (!server->getUser(clientTarget)->getChan(channel)) {
		msg(ERR_USERNOTINCHANNEL_F(getFullPrefix(), getNick(), channel));
		return false;
	}
	if (!server->getChan(channel)->Rights.getGive(getNick())) {
		msg(ERR_CHANOPRIVSNEEDED_F(getFullPrefix(), getNick(), channel));
		return false;
	}
	if (!(right == "write" || right == "quick" || right == "invit" || right == "modes" || right == "give")) {
		msg(ERR_UMODEUNKNOWNFLAG_F(channel, getNick()));
		return false;
	}
	if (!(checkbox == "yes" || checkbox == "no"))
		return false;
	return true;
}

void	IRCClient::handleSet(const std::string &params) {
	std::istringstream iss(params);

	std::string channel, clientTarget, right, checkbox, extraWord;

	if (iss >> channel && iss >> clientTarget && iss >> right && iss >> checkbox) {
		if (!(iss >> extraWord)) {
			if (isValidSetParams(channel, clientTarget, right, checkbox)) {
				giveRight(channel, clientTarget, right, checkbox);
				send(socket, "the modification is successful\n", 32, 0);
            }
		}	else
				msg(ERR_UNKNOWNCOMMAND_F(getFullPrefix(), getNick(), "SET"));		
	}	else
			msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "SET"));
}
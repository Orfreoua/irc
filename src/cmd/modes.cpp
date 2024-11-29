#include "headers.hpp"

void IRCClient::handleMode(const std::string &params) {
	std::istringstream iss(params);
	std::string channel, arg;

	if (!(iss >> channel && iss >> arg)) {
        if (server->getChan(channel)) {
            msg(RPL_CHANNELMODEIS_F(getFullPrefix(), getNick(), channel, server->getChan(channel)->listModes(server->getChan(channel)->Rights.getModes(this->getNick())))); return ;}
        msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE")); return ; }

	
	if (!server->getChan(channel)) {
		msg(ERR_NOSUCHCHANNEL_F(getFullPrefix(), channel));
		return;
	}

	if (!server->getChan(channel)->Rights.getModes(this->getNick()))
	{
		msg(ERR_CHANOPRIVSNEEDED_F(getFullPrefix(), getNick(), channel));
		return ;
	}

	if (arg == "-k" || arg == "+k") {
		handleModeKey(iss, arg, channel);
	} else if (arg == "-o" || arg == "+o") {
		handleModeOperator(iss, arg, channel);
	} else if (arg == "-l" || arg == "+l") {
		handleModeLimit(iss, arg, channel);
	} else if (arg == "-t" || arg == "+t") {
		handleModeTopic(iss, arg, channel);
	} else if (arg == "-i" || arg == "+i") {
		handleModeInvite(iss, arg, channel);
	} else
		msg(ERR_UNKNOWNMODE_F(getFullPrefix(), getNick(), arg));
}

void IRCClient::handleModeKey(std::istringstream &iss, const std::string &arg, const std::string &channel) {
	std::string pass, extraWord;
	if (arg == "-k") {
		if (!(iss >> extraWord)) {
			server->getChan(channel)->mode.pass = false;
			server->getChan(channel)->setMdp("");
			send(socket, CHANNEL_ACCESSIBLE_NO_PASSWORD, strlen(CHANNEL_ACCESSIBLE_NO_PASSWORD), 0);
		} else
		   msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
	} else if (arg == "+k") {
		if ((iss >> pass)) {
			if (!(iss >> extraWord)) {
				server->getChan(channel)->mode.pass = true;
				server->getChan(channel)->setMdp(pass);
				send(socket, MSG_PASSWORD_CHANGED, strlen(MSG_PASSWORD_CHANGED), 0);
			} else
				msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
		} else
			msg(ERR_BADCHANNELKEY_F(getFullPrefix(), channel));
	}
}

void IRCClient::handleModeOperator(std::istringstream &iss, const std::string &arg, const std::string &channel) {
	std::string useri, extraWord;
	if ((iss >> useri)) {
		if (!(iss >> extraWord)) {
			if (server->getChan(channel)->getUser(useri))
			{
				if (arg == "+o"){
					server->getChan(channel)->Rights.setRights(useri, server->getChan(channel)->Rights.RootRights());
					server->getUser(useri)->msg(MSG_OP_GRANTED);
					msg(MSG_OP_GRANTED_R);
				} else {
					server->getChan(channel)->Rights.setRights(useri, server->getChan(channel)->Rights.BasicRights());
					server->getUser(useri)->msg(MSG_OP_REVOKED);
					msg(MSG_OP_REVOKED_R);
				}
			} else
				msg(ERR_USERNOTINCHANNEL_F(getFullPrefix(), getNick(), channel));
		} else
			msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
	} else
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
}

void IRCClient::handleModeLimit(std::istringstream &iss, const std::string &arg, const std::string &channel) {
	std::string limit, extraWord;
	if (arg == "-l") {
		if (!(iss >> extraWord)) {
			server->getChan(channel)->mode.changeLimitUsers = false;
			server->getChan(channel)->mode.Vlimit = MAX_CLIENTS;
			send(socket, MSG_MAX_CLIENTS_DISABLED, strlen(MSG_MAX_CLIENTS_DISABLED), 0);
		} else
			msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
	} else if (arg == "+l") {
		if ((iss >> limit)) {
			std::istringstream issLimit(limit);
			int intValue;
			if (issLimit >> intValue && intValue >= 0)
			{
				server->getChan(channel)->mode.Vlimit = intValue;
				server->getChan(channel)->mode.changeLimitUsers = true;
				send(socket, MSG_MAX_CLIENTS_SET, strlen(MSG_MAX_CLIENTS_SET), 0);
			}
			else
				msg(ERR_UNKNOWNMODE_F(getFullPrefix(), getNick(), limit));
		} else
			msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
	}
}

void IRCClient::handleModeTopic(std::istringstream &iss, const std::string &arg, const std::string &channel) {
	std::string extraWord;

	if (!(iss >> extraWord)) {
		if (arg == "+t") {
			server->getChan(channel)->mode.topic = true;
			send(socket, MSG_TOPIC_VISIBLE, strlen(MSG_TOPIC_VISIBLE), 0);
		}
		else
		{
			server->getChan(channel)->mode.topic = false;
			send(socket, MSG_TOPIC_OP_ONLY, strlen(MSG_TOPIC_OP_ONLY), 0);
		}
	} else
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
}

void IRCClient::handleModeInvite(std::istringstream &iss, const std::string &arg, const std::string &channel) {
	std::string extraWord;

	if (!(iss >> extraWord)) {
		if (arg == "+i") {
			server->getChan(channel)->mode.invitOnly = true;
			send(socket, MSG_CHANNEL_INVITE_ONLY, strlen(MSG_CHANNEL_INVITE_ONLY), 0);
		}
		else {
			server->getChan(channel)->mode.invitOnly = false;
			send(socket, MSG_CHANNEL_OPEN, strlen(MSG_CHANNEL_OPEN), 0);
		}
	} else
		msg(ERR_NEEDMOREPARAMS_F(getFullPrefix(), "MODE"));
}

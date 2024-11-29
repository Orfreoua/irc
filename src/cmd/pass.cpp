#include "headers.hpp"

void IRCClient::handlePass(const std::string &params) {
    // Check if the user is already registered
    if (isAuthenticated) {
        std::vector<std::string> errorMsgParams;
        this->server->sendFormattedReply(this->socket, ERR_ALREADYREGISTERED, errorMsgParams, "You may not reregister");
        return;
    }
    std::istringstream iss(params);
    std::string pass;
    // Extract the password from the parameters
    if (!(iss >> pass)) {
        std::cerr << "Invalid PASS command format." << std::endl;
        std::vector<std::string> errorMsgParams;
        errorMsgParams.push_back("PASS");
        this->server->sendFormattedReply(this->socket, ERR_NEEDMOREPARAMS, errorMsgParams, "Not enough parameters");
        return;
    }
    std::string extra;
    if (iss >> extra) {
        // There are extra parameters beyond the first one
        std::cerr << "Too many parameters for PASS command." << std::endl;
        std::vector<std::string> errorMsgParams;
        errorMsgParams.push_back("PASS");
        this->server->sendFormattedReply(this->socket, ERR_UNKNOWNCOMMAND, errorMsgParams, "Too many parameters");
        return;
    }
    // Check if the password meets security requirements
    if (!isValidPass(pass)) {
        std::cerr << "Password does not meet security requirements." << std::endl;
        std::vector<std::string> errorMsgParams;
        this->server->sendFormattedReply(this->socket, ERR_PASSWDMISMATCH, errorMsgParams, "Password does not meet security requirements");
        return;
    }
    // Verify the password with the server
    if (this->server->verifyPass(pass, this->socket)) {
        std::cout << "Authentication successful." << std::endl;
        this->isAuthenticated = true;
    } else {
        std::cerr << "Authentication failed: incorrect password." << std::endl;
        std::vector<std::string> errorMsgParams;
        this->server->sendFormattedReply(this->socket, ERR_PASSWDMISMATCH, errorMsgParams, "Incorrect password");
    }
}

bool IRCClient::isValidPass(const std::string &password) {
  return password.length() >= 2; // Enforce a minimum length for security
}

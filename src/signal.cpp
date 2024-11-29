#include "headers.hpp"

void quit(int sig_num) {
    if (sig_num == SIGINT) {
        std::cout << "\b\bShutting Down Server..." << std::endl;
        //IRCServer::getInstance().closeAllClients();
        std::ostringstream oss;
        oss << IRCServer::getInstance().getPort();
        std::string portStr = oss.str();
        std::string command = "pkill -9 -f \":" + portStr + "\" > /dev/null 2>&1";
        system(command.c_str());
        is_running = 0;
        //exit(EXIT_FAILURE);
    }
}


void signal_catching() {
  signal(SIGINT, quit);
  signal(SIGQUIT, SIG_IGN);
}

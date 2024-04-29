#include "signal.hpp"

void	quit(int sig_num)
{
	if (sig_num == SIGINT)
	{
		std::cout << "\b\bShutting down server" << std::endl;
		std::ostringstream oss;
		oss << port;
		std::string portStr = oss.str();
		std::string command = "pkill -9 -f \":" + portStr + "\" > /dev/null 2>&1";
		//std::cout << "Executing command: " << command << std::endl;
		system(command.c_str());
		exit(0);
	}
}

void	signal_catching()
{
	signal(SIGINT, quit);
	signal(SIGQUIT, SIG_IGN);
}
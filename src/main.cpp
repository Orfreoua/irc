#include "IRCServer.hpp"
#include "signal.hpp"

#define MAX_CLIENTS 1000 // Nombre maximal de clients pouvant être connectés
#define BUFFER_SIZE 1024 // Taille du buffer

int port;

int main(int argc, char* argv[]) 
{
	//-- gestion de la sortie via signal
	signal_catching();

    if (argc != 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    port = std::atoi(argv[1]);
    std::string password = argv[2];

    try
    {
        IRCServer server;

        //server.setIpProtocol(AF_INET6);
        server.setPort(port);
        server.setPassword(password);

        if(!server.initSocket()) { throw std::runtime_error("Failed to init socket."); }

        if(!server.bindSocket()) { throw std::runtime_error("Failed to bind socket."); }

        if(!server.isListening()) { throw std::runtime_error("Failed to listen connections."); }
    
        server.acceptClients();
    } 
    catch (const std::runtime_error& e) 
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
    
}


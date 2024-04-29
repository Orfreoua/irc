
class ClientHandler {
private:
    ClientManager& manager;
    int clientSocket;
    ClientInfo clientInfo;

public:
    ClientHandler(ClientManager& manager, int socket, const std::string& host)
        : manager(manager), clientSocket(socket), clientInfo(socket, host) {}

    static void* handleClient(void* arg) {
        ClientHandler* handler = static_cast<ClientHandler*>(arg);
        handler->processClient();
        delete handler;
        return NULL;
    }

   
   
   
    void processClient() {
        std::cout << "Handling new client on socket " << clientSocket << std::endl;
        sendToClient(clientSocket, "Please enter your nickname: ");

        char nick[BUFFER_SIZE];
        if (recv(clientSocket, nick, BUFFER_SIZE - 1, 0) <= 0) {
            std::cerr << "Failed to receive nickname from client on socket " << clientSocket << std::endl;
            close(clientSocket);
            return;
        }
        nick[strlen(nick) - 1] = '\0';  // Remove newline character if any

        clientInfo.nick = nick;  // Set nickname
        manager.addClient(clientSocket, clientInfo);

        std::string welcomeMsg = ":Server 42 " + std::string(nick) + " :Welcome to my server on the IRC network!\n";
        sendToClient(clientSocket, welcomeMsg);

        char buffer[BUFFER_SIZE];
        while (true) {
            ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesRead <= 0) {
                std::cerr << "Error or connection closed by client on socket " << clientSocket << std::endl;
                break;
            }
            buffer[bytesRead] = '\0';
            std::string message = ":" + clientInfo.nick + "!" + clientInfo.user + "@" + clientInfo.host +
                                  " PRIVMSG " + clientInfo.channel + " :" + std::string(buffer) + "\n";
            manager.broadcastMessage(clientInfo.channel, message, clientSocket);
        }

        close(clientSocket);
        manager.removeClient(clientSocket);
        std::cout << "Closed connection with client on socket " << clientSocket << std::endl;
    }



private:
    void sendToClient(int socket, const std::string& message) {
        if (send(socket, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Error sending message to client on socket " << socket << std::endl;
        }
    }
};

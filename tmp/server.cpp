// server.cpp


#define IP_PROTOCOL AF_INET6

#include "server.hpp"

std::map<int, ClientInfo> clients;
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

void broadcastMessage(const std::string& channel, const std::string& message, int senderSocket) {
    pthread_mutex_lock(&clientsMutex);
    std::map<int, ClientInfo>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.channel == channel) {
            if (it->second.socket != senderSocket) {
                send(it->second.socket, message.c_str(), message.size(), 0);
            }
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}

void* handleClient(void* arg) {
    int clientSocket = *((int*)arg);
    char nick[BUFFER_SIZE];

    ssize_t bytesRead = recv(clientSocket, nick, BUFFER_SIZE - 1, 0);
    if (bytesRead <= 0) {
        std::cerr << "Error: Failed to receive nick from client." << std::endl;
        close(clientSocket);
        pthread_exit(NULL);
    }
    nick[bytesRead] = '\0'; // Adds the null-terminating character

    std::cout << "Client " << nick << " connected." << std::endl;

    char buffer[BUFFER_SIZE];
    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0'; // Adds the null-terminating character
        std::cout << "Client " << nick << " message: " << buffer << std::endl;
        broadcastMessage(clients[clientSocket].channel, buffer, clientSocket);
    }

    close(clientSocket);
    pthread_exit(NULL);
}

void runServer(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    int serverSocket = socket(IP_PROTOCOL, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Failed to create socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in6 serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = htons(port);
    serverAddress.sin6_addr = in6addr_any;
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: Failed to bind socket to port " << port << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error: Failed to listen for connections." << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << std::endl;
    while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            std::cerr << "Error: Failed to accept connection." << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, (void*)&clientSocket) != 0) {
            std::cerr << "Error: Failed to create thread." << std::endl;
            close(clientSocket);
        } else {
            pthread_mutex_lock(&clientsMutex);
            ClientInfo clientInfo;
            clientInfo.socket = clientSocket;
            clients[clientSocket] = clientInfo;
            pthread_mutex_unlock(&clientsMutex);
        }
    }

    close(serverSocket);
}


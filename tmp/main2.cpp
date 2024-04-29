#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <map>
#include <string>

#define BUFFER_SIZE 1024






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


    // Votre logique principale ici
    runServer(argc, argv);

    return EXIT_SUCCESS;
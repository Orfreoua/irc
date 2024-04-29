#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <map>
#include <string>

#define BUFFER_SIZE 1024

#define RED     "\033[31m"      
#define GREEN   "\033[32m"      
#define RESET   "\033[0m"

#define IP_PROTOCOL AF_INET6

// Structure to hold client information
struct ClientInfo {
	int socket;
	std::string nick;
	std::string channel;
};

// Map to store connected clients, mapped by socket
std::map<int, ClientInfo> clients;

// Mutex for thread-safe access to clients map
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

// Function to broadcast a message to all clients in the same channel
void broadcastMessage(const std::string& channel, const std::string& message, int senderSocket) {
	pthread_mutex_lock(&clientsMutex);
	std::map<int, ClientInfo>::const_iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		if (it->second.channel == channel) {
			// Skip the sender client
			if (it->second.socket != senderSocket) {
				send(it->second.socket, message.c_str(), message.size(), 0);
			}
		}
	}
	pthread_mutex_unlock(&clientsMutex);
}

// Function to handle client communication
void* handleClient(void* arg) {
	int clientSocket = *((int*)arg);
	char nick[BUFFER_SIZE];

	// Receives the nick from the client
	ssize_t bytesRead = recv(clientSocket, nick, BUFFER_SIZE - 1, 0);
	if (bytesRead <= 0) {
		std::cerr << "Error: Failed to receive nick from client." << std::endl;
		close(clientSocket);
		pthread_exit(NULL);
	}
	nick[bytesRead] = '\0'; // Adds the null-terminating character

	std::cout << "Client " << nick << " connected." << std::endl;

	// Reads and displays messages from the client
	char buffer[BUFFER_SIZE];
	while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
		buffer[bytesRead] = '\0'; // Adds the null-terminating character
		std::cout << "Client " << nick << " message: " << buffer << std::endl;
		// Broadcast the message to all clients in the same channel
		broadcastMessage(clients[clientSocket].channel, buffer, clientSocket);
	}

	// Closes the connection with the client
	close(clientSocket);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	// Verification of the number of arguments
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}

	// Retrieval of the port and password
	int port = std::atoi(argv[1]);
	std::string password = argv[2];

	// Creation of the socket
	int serverSocket = socket(IP_PROTOCOL, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "Error: Failed to create socket." << std::endl;
		return EXIT_FAILURE;
	}
	
	// Binding the socket to the specified port
	struct sockaddr_in6 serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin6_family = AF_INET6; // Utilise IPv6
	serverAddress.sin6_port = htons(port); // Port (en ordre d'octets en réseau)
	serverAddress.sin6_addr = in6addr_any; //Utilise toutes les interfaces disponibles
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Error: Failed to bind socket to port " << port << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;
	}

	// Listening for incoming connections	
	if (listen(serverSocket, 5) == -1) {
		std::cerr << "Error: Failed to listen for connections." << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;
	}

	std::cout << "Server listening on port " << port << std::endl;
	// Accepting and responding to incoming connections	
	while (true) {
		int clientSocket = accept(serverSocket, NULL, NULL);
		if (clientSocket == -1) {
			std::cerr << "Error: Failed to accept connection." << std::endl;
			close(serverSocket);
			return EXIT_FAILURE;
		}

		// Create a new thread to handle the client connection
		pthread_t thread;
		if (pthread_create(&thread, NULL, handleClient, (void*)&clientSocket) != 0) {
			std::cerr << "Error: Failed to create thread." << std::endl;
			close(clientSocket);
		} else {
			// Add the client to the map
			pthread_mutex_lock(&clientsMutex);
			ClientInfo clientInfo;
			clientInfo.socket = clientSocket;
			clients[clientSocket] = clientInfo;
			pthread_mutex_unlock(&clientsMutex);
		}
	}

	close(serverSocket);
	return EXIT_SUCCESS;
}

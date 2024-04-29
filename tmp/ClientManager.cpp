class ClientManager {
private:
    std::map<int, ClientInfo> clients;
    pthread_mutex_t clientsMutex;

public:
    ClientManager() {
        pthread_mutex_init(&clientsMutex, NULL);
    }

    ~ClientManager() {
        pthread_mutex_destroy(&clientsMutex);
    }

    void addClient(int socket, const ClientInfo& info) {
        pthread_mutex_lock(&clientsMutex);
        clients[socket] = info;
        pthread_mutex_unlock(&clientsMutex);
    }

    void removeClient(int socket) {
        pthread_mutex_lock(&clientsMutex);
        clients.erase(socket);
        pthread_mutex_unlock(&clientsMutex);
    }

    void broadcastMessage(const std::string& channel, const std::string& message, int senderSocket) {
        pthread_mutex_lock(&clientsMutex);
        for (std::map<int, ClientInfo>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.channel == channel && it->second.socket != senderSocket) {
                send(it->second.socket, message.c_str(), message.size(), 0);
            }
        }
        pthread_mutex_unlock(&clientsMutex);
    }
};
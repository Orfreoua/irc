#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "headers.hpp"

class IRCClient;

typedef struct {
    bool invitOnly; // -i
    bool topic; // -t
    bool pass; // -k
    bool setModes; // -o
    bool changeLimitUsers; // -l
    int  Vlimit;
} t_modes;

class Channel {
private:
    std::string chan_name;
    std::string mdp;
    std::map<std::string, IRCClient*> users;
    int r, g, b;
    std::string topic;
    std::string t_time;
    std::string t_who;
    bool invite;
//    t_modes mode;

public:
    int maxClients;
    rights Rights;
    t_modes mode;
    t_modes BasicMode();

    Channel(std::string chan_name, std::string password);
    ~Channel();

    void addUser(int userSocket, std::string nick);
    void rmUser(std::string nick);
    std::string writeUsers();
    void printChan();
    void printMsg_u(std::string from);
    void printMsg_s(std::string from);

    bool testMdp(std::string pass);

    bool getUser(std::string user);
    std::string getChan();
    std::string getRGB();
    std::string getTopic();
    void setRGB(int rgb);
    void setTopic(std::string message);
    int getNbCli();
    void setTopicWhoTime(std::string nick);
    std::string getCurrentTimestamp();
    std::string get_t_who();
    std::string get_t_time();

    void msg(std::string str);
    void msg_n(std::string	old, std::string now);
    void msg_o(std::string	str);

    void msg_from(std::string	str, std::string nick);
    void setMdp(const std::string& newMdp);
    std::string getMdp() { return mdp; }
    std::string    listModes(bool is_operator);
};

#endif
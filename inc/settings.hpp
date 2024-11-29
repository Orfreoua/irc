#ifndef SETTINGS_HPP
#define SETTINGS_HPP


#ifdef __linux__
#define _LINUX
#elif __APPLE__
#define _MACOS
#endif

#ifdef _LINUX
#define OS_VERSION 0
#endif

#ifdef _MACOS
#define OS_VERSION 1
#endif


#define MAX_CLIENTS 1000 // Nombre maximal de clients pouvant être connectés
#define BUFFER_SIZE 1024 // Taille du buffer

#define IP_PROTOCOL AF_INET6



#define USER "\033[38;2;250;232;140m"
#define CHAN "\033[38;2;135;206;250m"
#define BASICCHAN 135206250
#define NORMAL "\033[0m"
#define REDLIGHT "\033[38;5;203m"
#define PMCOLOR	"\033[125;51;255]"


#define CHANNEL_ACCESSIBLE_NO_PASSWORD "The channel is now accessible without a password.\n"
#define MSG_PASSWORD_CHANGED "Password for the channel has been successfully changed.\n"
#define MSG_OP_GRANTED "You are now an operator of the channel.\n"
#define MSG_OP_GRANTED_R "You promoted someone operator of the channel.\n"
#define MSG_OP_REVOKED "You are no longer an operator of the channel.\n"
#define MSG_OP_REVOKED_R "You revoqued an operator of the channel.\n"
#define MSG_MAX_CLIENTS_SET "Maximum client limit on the server has been set.\n"
#define MSG_MAX_CLIENTS_DISABLED "Maximum client limit on the server has been disabled.\n"
#define MSG_TOPIC_VISIBLE "The topic is now visible to all users.\n"
#define MSG_TOPIC_OP_ONLY "The topic is now accessible to operators only.\n"
#define MSG_CHANNEL_OPEN "The channel is now open.\n"
#define MSG_CHANNEL_INVITE_ONLY "The channel is now accessible by invitation only.\n"


#endif
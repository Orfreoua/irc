#ifndef CHANNELINFO_HPP
#define CHANNELINFO_HPP

#ifndef CHANNEL_INFO_HPP
#define CHANNEL_INFO_HPP

#include <string>
#include <vector>
#include <map>

struct ChannelInfo {
    std::string name;
    std::string password;
    std::vector<std::string> members;
    std::string topic;
    std::map<std::string, bool> modes;

    ChannelInfo(const std::string& name, const std::string& password) : name(name), password(password) {}
};

#endif // CHANNEL_INFO_HPP

#endif
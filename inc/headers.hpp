#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sodium.h>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <vector>

#include "rights.hpp"
#include "IRCChannel.hpp"
#include "IRCServerCapabilities.hpp"
#include "IRCServer.hpp"
#include "IRCClient.hpp"
#include "settings.hpp"
#include "signal.hpp"
#include "cleanMap.tpp"

extern int  is_running;

#endif
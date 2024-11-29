#include "headers.hpp"

IRCServerCapabilities::IRCServerCapabilities() {
  add("multi-prefix", "enabled");
}

void IRCServerCapabilities::add(const std::string &cap,
                                const std::string &description) {
  capabilities[cap] = description;
}

std::string IRCServerCapabilities::formatForClient() const {
  std::string formatted;
  for (std::map<std::string, std::string>::const_iterator it =
           capabilities.begin();
       it != capabilities.end(); ++it) {
    if (!formatted.empty()) {
      formatted += ", ";
    }
    formatted += it->first + "=" + it->second;
  }
  return formatted;
}

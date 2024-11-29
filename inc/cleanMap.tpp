#ifndef CLEAN_MAP_TPP
#define CLEAN_MAP_TPP

template<typename T>
void cleanMap(std::map<T, IRCClient*>& container) {
    typename std::map<T, IRCClient*>::iterator it;
    for (it = container.begin(); it != container.end(); ++it) {
        delete it->second;
    }
    container.clear();
}

#endif
#include "list.h"
#include <cstring>     // dla memset()
#include <arpa/inet.h> // dla inet_pton()
#include <cstdio>      // dla perror()
#include <stdexcept>   // dla std::out_of_range

// The ListOfSockaddr class encapsulates a dynamic list of IPv4 addresses with ports.
// It provides methods to add, access, and remove entries in a type-safe manner.

ListOfSockaddr::ListOfSockaddr() {
    // Default constructor: initializes an empty list of socket addresses.
}

ListOfSockaddr::~ListOfSockaddr() {
    // Destructor: std::vector automatically manages its own memory, so no manual cleanup is needed.
}

// Adds a new IPv4 address and port to the list.
// Returns true on success, or false if the IP conversion fails.
bool ListOfSockaddr::add(const std::string &ip, uint16_t port) {
    struct sockaddr_in destAddr;
    std::memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    // Konwersja adresu IP (IPv4) z formatu tekstowego na postać binarną.
    if (inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr) <= 0) {
        std::perror("inet_pton failed");
        return false;
    }
    addresses.push_back(destAddr);
    return true;
}

// Returns a const reference to the internal vector of socket addresses.
const std::vector<struct sockaddr_in>& ListOfSockaddr::getList() const {
    return addresses;
}

// Returns a const reference to the element at the specified index.
// Throws std::out_of_range if the index is invalid.
const struct sockaddr_in& ListOfSockaddr::getElement(size_t index) const {
    if (index >= addresses.size()) {
        throw std::out_of_range("Index out of range in ListOfSockaddr::getElement");
    }
    return addresses[index];
}

// Overloads operator[] to provide indexed access, delegating to getElement().
const struct sockaddr_in& ListOfSockaddr::operator[](size_t index) const {
    return getElement(index);
}

// Returns the number of socket addresses currently stored.
size_t ListOfSockaddr::size() const {
    return addresses.size();
}

// Clears all entries from the list.
void ListOfSockaddr::clear() {
    addresses.clear();
}

// Removes the entry at the specified index.
// Throws std::out_of_range if the index is invalid.
void ListOfSockaddr::remove(size_t index) {
    if (index >= addresses.size()) {
        throw std::out_of_range("Index out of range in ListOfSockaddr::remove");
    }
    // Erase the element safely using a vector iterator.
    using diff_t = std::vector<struct sockaddr_in>::difference_type;
    diff_t idx = static_cast<diff_t>(index);
    addresses.erase(addresses.begin() + idx);
}

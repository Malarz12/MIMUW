#ifndef LIST_H
#define LIST_H

#include <netinet/in.h>
#include <string>
#include <vector>
#include <stdexcept>

// The ListOfSockaddr class manages a dynamic collection of IPv4 addresses with associated ports.
// It provides methods for adding, accessing, and removing entries in a safe and type-checked way.
class ListOfSockaddr {
public:
    // Constructs an empty list of socket addresses.
    ListOfSockaddr();

    // Destroys the list, automatically freeing any associated resources.
    ~ListOfSockaddr();

    // Adds a new IPv4 address and port to the list.
    // Returns true if the address and port were successfully added, or false on failure (e.g., invalid IP string).
    bool add(const std::string &ip, uint16_t port);

    // Provides read-only access to the internal std::vector of sockaddr_in structures.
    // This method does not allow modification of the underlying list.
    const std::vector<struct sockaddr_in>& getList() const;

    // Returns a constant reference to the sockaddr_in entry at the given index.
    // Throws std::out_of_range if the index is outside the valid range.
    const struct sockaddr_in& getElement(size_t index) const;

    // Overloaded subscript operator to access elements by index.
    // It delegates to getElement() and will throw std::out_of_range on invalid index.
    const struct sockaddr_in& operator[](size_t index) const;

    // Returns the number of socket addresses currently stored in the list.
    size_t size() const;

    // Clears all entries from the list, resulting in an empty list.
    void clear();

    // Removes the entry at the specified index from the list.
    // Throws std::out_of_range if the index is invalid.
    void remove(size_t index);

private:
    // Internal storage for the list of IPv4 addresses and ports.
    std::vector<struct sockaddr_in> addresses;
};

#endif // LIST_H

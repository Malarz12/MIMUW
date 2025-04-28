#ifndef HELPERS_H
#define HELPERS_H

#include <stdexcept>
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <climits>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>

#include "err.h"
#include "list.h"
#include "message.h"

namespace helpers {

// Structure to hold command-line options provided by the user.
// It includes the local bind address, port, and optional peer information.
struct Options {
    std::string bind_address = "0.0.0.0";
    uint16_t port = 0;
    std::string peer_address;
    uint16_t peer_port = 0;
};

// Structure representing a peer node used for synchronization logic.
// It tracks whether the node exists, its address, port, sync level, and last contact time.
struct Node {
    bool does_exist = false;
    uint16_t port = 0;
    std::string address;
    int synchronization = 255;
    std::chrono::steady_clock::time_point last_heard = std::chrono::steady_clock::now();
};

// Converts a C-string to a valid port number. It terminates the program on invalid input.
inline uint16_t read_port(const char *str) {
    char *endptr;
    errno = 0;
    unsigned long val = strtoul(str, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val > UINT16_MAX) {
        fatal("not valid port");
    }
    return static_cast<uint16_t>(val);
}

// Parses the command-line arguments and populates the Options struct accordingly.
inline void parse_arguments(int argc, char *argv[], Options &opts) {
    int ch;
    while ((ch = getopt(argc, argv, "b:p:a:r:")) != -1) {
        switch (ch) {
            case 'b': opts.bind_address = optarg; break;
            case 'p': opts.port = read_port(optarg); break;
            case 'a': opts.peer_address = optarg; break;
            case 'r': opts.peer_port = read_port(optarg); break;
            default: fatal("wrong flag");
        }
    }
}

// Creates a non-blocking UDP socket and binds it to the specified address and port.
// On any system error, the program is terminated.
inline int create_and_bind_socket(const Options &opts) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) syserr("cannot create socket");
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        syserr("fcntl F_GETFL");
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        syserr("fcntl F_SETFL");
    }


    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(opts.port);
    if (opts.bind_address == "0.0.0.0") {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (inet_pton(AF_INET, opts.bind_address.c_str(), &addr.sin_addr) <= 0) {
        syserr("invalid bind address");
    }

    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        syserr("bind failed on");
    }
    return fd;
}

// Sends an initial HELLO message to the configured peer, if any peer was specified.
inline void send_initial_hello(int socket_fd, const Options &opts) {
    if (opts.peer_address.empty() || opts.peer_port == 0) return;
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(opts.peer_port);
    if (inet_pton(AF_INET, opts.peer_address.c_str(), &dest.sin_addr) <= 0) {
        syserr("invalid peer address");
    }
    Message msg = make_HELLO();
    auto buf = serialize(msg);
    if (sendto(socket_fd, buf.data(), buf.size(), 0,
               reinterpret_cast<sockaddr*>(&dest), sizeof(dest)) < 0) {
        syserr("sendto HELLO failed");
    }
}

// Checks whether the current synchronization source has timed out.
// If the node fails to respond within 20 seconds, it is invalidated.
inline void check_source_timeout(Node &src, bool leader, int &synchronization) {
    auto now = std::chrono::steady_clock::now();
    if (!leader && src.does_exist &&
        std::chrono::duration_cast<std::chrono::seconds>(now - src.last_heard) > std::chrono::seconds(20)) {
        src.does_exist = false;
        synchronization = 255;
    }
}

// Serializes a Message, sends it over UDP, and returns false on any error.
inline bool send_message(int socket_fd,
    const Message &msg,
    const sockaddr_in &dest,
    const std::string &what) {
    try {
        auto buf = serialize(msg);
        ssize_t sent = sendto(socket_fd,
                reinterpret_cast<const char*>(buf.data()), buf.size(),
                0,
                reinterpret_cast<const sockaddr*>(&dest),
                sizeof(dest));
        if (sent < 0) {
            error("sendto failed for " + what);
            return false;
        }
        return true;
    } catch (const std::exception &ex) {
        // np. przekroczono rozmiar UDP albo za dużo węzłów
        error(std::string("send_message: ") + ex.what());
        return false;
    }
}

// Sends SYNC_START messages to all known peers every 5 seconds if not fully synchronized.
inline void try_send_start_sync(int socket_fd,
                                ListOfSockaddr &addrList,
                                int &synchronization,
                                std::chrono::steady_clock::time_point &last_start,
                                std::chrono::steady_clock::time_point start_time,
                                std::chrono::steady_clock::duration offset) {
    auto now = std::chrono::steady_clock::now();
    if (synchronization < 254 &&
        std::chrono::duration_cast<std::chrono::seconds>(now - last_start) > std::chrono::seconds(5)) {
        last_start = now;
        for (size_t i = 0; i < addrList.size(); ++i) {
            sockaddr_in dest = addrList.getElement(i);
            Message msg = make_SYNC_START(synchronization, start_time, offset);
            send_message(socket_fd, msg, dest, "SYNC_START");
        }
    }
}

// Returns true if the given ip and port match a known peer in addrList.
inline bool is_known_peer(const ListOfSockaddr &addrList,
                          const std::string &ip,
                          uint16_t port) {
    for (size_t i = 0; i < addrList.size(); ++i) {
        const sockaddr_in &peer = addrList.getElement(i);
        if (ntohs(peer.sin_port) != port) continue;
        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &peer.sin_addr, buf, sizeof(buf)) && ip == buf) {
            return true;
        }
    }
    return false;
}

// Validates the contacts array in a HELLO_REPLY and adds them only if all records are valid.
inline bool handle_hello_reply_contacts(const Message &recvMsg,
    const std::string &sender_ip,
    uint16_t sender_port,
    const std::string &local_ip,
    uint16_t local_port,
    ListOfSockaddr &receivedContacts) 
{
    // 1) Count must match actual number of nodes.
    if (recvMsg.nodes.size() != recvMsg.count) return false;

    // Temporary storage for validated contacts.
    std::vector<std::pair<std::string,uint16_t>> temp;
    temp.reserve(recvMsg.count);

    for (auto const &rec : recvMsg.nodes) {
        // 2) peer_address_length matches vector size and equals 4 (IPv4).
        if (rec.peer_address_length != rec.peer_address.size()) return false;
        uint16_t port = ntohs(rec.peer_port);
        // 3) peer_port must be non-zero
        if (port == 0) return false;

        // 4) Convert raw address to string.
        char ip_str[INET_ADDRSTRLEN];
        sockaddr_in inaddr{};
        memcpy(&inaddr.sin_addr, rec.peer_address.data(), rec.peer_address_length);
        if (!inet_ntop(AF_INET, &inaddr.sin_addr, ip_str, sizeof(ip_str))) return false;
        std::string ip(ip_str);

        // 5) Exclude sender and local bind address.
        if ((ip == sender_ip && port == sender_port) ||
            (ip == local_ip  && port == local_port)) {
            return false;
        }

        // 6) Valid record: save for later.
        temp.emplace_back(ip, port);
    }

    // All records validated: add them to receivedContacts.
    for (auto const &entry : temp) {
        receivedContacts.add(entry.first.c_str(), entry.second);
    }

    return true;
}
// Sends CONNECT messages to all contacts stored in receivedContacts and clears the list.
inline bool send_connects(int socket_fd,
                          ListOfSockaddr &receivedContacts) {
    bool result = true;
    while (receivedContacts.size() > 0) {
        sockaddr_in contact = receivedContacts.getElement(0);
        Message connectMsg = make_CONNECT();
        if(!send_message(socket_fd, connectMsg, contact, "CONNECT")){
            error("message not send");
            result = false;
        };
        receivedContacts.remove(0);
    }
    return result;
}

} // namespace helpers

#endif
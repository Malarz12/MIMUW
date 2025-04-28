#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "err.h"
#include "message.h"

int main(int argc, char *argv[]) {
    // Ensure correct usage: program expects exactly three arguments.
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <peer IP> <peer port> <synchronization level>\n";
        return EXIT_FAILURE;
    }

    // Extract the peer IP address string
    const char *ip = argv[1];

    // Parse and validate the port number argument
    char *endptr = nullptr;
    errno = 0;
    unsigned long port_val = std::strtoul(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || port_val > UINT16_MAX) {
        fatal("Invalid port number provided");
    }
    uint16_t port = static_cast<uint16_t>(port_val);

    // Parse and validate the synchronization level argument.
    errno = 0;
    long sync_val = std::strtol(argv[3], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || sync_val < 0 || sync_val > 255) {
        fatal("Invalid synchronization level provided");
    }
    int sync_level = static_cast<int>(sync_val);

    // Create a UDP socket for communication.
    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        syserr("socket");
    }

    // Prepare the destination sockaddr_in structure.
    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &dest.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip << "\n";
        ::close(sock);
        return EXIT_FAILURE;
    }

    // Build a LEADER message with the specified synchronization level.
    Message msg = make_LEADER(sync_level);
    msg.refresh_count();  // Ensure count field matches nodes vector size.

    // Serialize the message to a byte buffer.
    std::vector<uint8_t> buf = serialize(msg);

    // Send the serialized LEADER message over UDP.
    ssize_t sent_bytes = ::sendto(
        sock,
        reinterpret_cast<const char*>(buf.data()),
        buf.size(),
        0,
        reinterpret_cast<const struct sockaddr*>(&dest),
        sizeof(dest)
    );
    if (sent_bytes < 0) {
        syserr("sendto");
    }

    // Report success to stdout.
    std::cout << "Sent LEADER to " << ip << ":" << port
              << " (" << sent_bytes << " bytes)\n";

    // Close the socket before exiting.
    if (::close(sock) < 0) {
        syserr("close");
    }

    return EXIT_SUCCESS;
}

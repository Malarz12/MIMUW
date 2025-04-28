#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <chrono>

#include "err.h"
#include "message.h"

int main(int argc, char *argv[]) {
    // This program expects pairs of IP addresses and ports as command-line arguments.
    if (argc < 3 || (argc % 2) == 0) {
        std::cerr << "Usage: " << argv[0]
                  << " <ip1> <port1> [<ip2> <port2> ...]" << std::endl;
        return EXIT_FAILURE;
    }

    // We will store each peer's sockaddr_in structure in this vector.
    std::vector<sockaddr_in> peers;

    // Parse the command-line arguments into the peers vector.
    for (int i = 1; i < argc; i += 2) {
        const char *ip = argv[i];
        const char *port_str = argv[i + 1];
        char *endptr = nullptr;
        errno = 0;

        // Convert the port string to an unsigned long and validate it.
        unsigned long p = strtoul(port_str, &endptr, 10);
        if (errno != 0 || *endptr != '\0' || p > UINT16_MAX) {
            std::cerr << "Invalid port: " << port_str << std::endl;
            return EXIT_FAILURE;
        }
        uint16_t port = static_cast<uint16_t>(p);

        // Initialize a sockaddr_in structure for this peer.
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_port   = htons(port);

        // Convert the IP address string to a binary representation.
        if (inet_pton(AF_INET, ip, &dest.sin_addr) <= 0) {
            std::cerr << "Invalid IP: " << ip << std::endl;
            return EXIT_FAILURE;
        }

        // Add the initialized structure to the peers vector.
        peers.push_back(dest);
    }

    // Create a non-blocking UDP socket for sending and receiving.
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        syserr("socket");
    }
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    // Send a GET_TIME request message to each peer.
    for (const auto &dest : peers) {
        Message req = make_GET_TIME();
        req.refresh_count();  // Ensure the node count is up to date.
        auto buf = serialize(req);

        // Transmit the serialized message via UDP.
        if (sendto(sock,
                   reinterpret_cast<const char*>(buf.data()), buf.size(),
                   0,
                   reinterpret_cast<const sockaddr*>(&dest),
                   sizeof(dest)) < 0) {
            syserr("sendto GET_TIME");
        }
    }

    // Prepare storage for tracking which peers have responded.
    std::vector<bool> received(peers.size(), false);
    std::vector<Message> responses(peers.size());

    // Record the start time and set a 10 ms timeout for collecting replies.
    auto start = std::chrono::steady_clock::now();
    const auto timeout = std::chrono::milliseconds(10);
    std::vector<uint8_t> recv_buf(65536);

    // Loop until the timeout expires, collecting incoming messages.
    while (std::chrono::steady_clock::now() - start < timeout) {
        sockaddr_in src_addr{};
        socklen_t addrlen = sizeof(src_addr);

        // Attempt to receive a UDP packet.
        ssize_t recvd = recvfrom(sock,
                                 reinterpret_cast<char*>(recv_buf.data()),
                                 recv_buf.size(),
                                 0,
                                 reinterpret_cast<sockaddr*>(&src_addr),
                                 &addrlen);
        if (recvd < 0) {
            // If no data is available or was interrupted, continue looping.
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
            // On any other error, report and exit.
            syserr("recvfrom");
        }

        // Attempt to deserialize the received data into a Message object.
        Message resp;
        try {
            resp = deserialize(recv_buf.data(), static_cast<size_t>(recvd));
        } catch (const std::exception &ex) {
            // If parsing fails, log the error and ignore this packet.
            std::cerr << "Failed to parse message: " << ex.what() << std::endl;
            continue;
        }

        // We only care about TIME (type 32) responses.
        if (resp.message != 32) {
            continue;
        }

        // Match the source address to our list of peers.
        for (size_t i = 0; i < peers.size(); ++i) {
            if (!received[i] &&
                peers[i].sin_addr.s_addr == src_addr.sin_addr.s_addr &&
                peers[i].sin_port == src_addr.sin_port) {
                // Record the received response for this peer.
                received[i]   = true;
                responses[i] = resp;
                break;
            }
        }
    }

    // After timeout, print all successfully received TIME responses.
    for (size_t i = 0; i < peers.size(); ++i) {
        if (received[i]) {
            char ipbuf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &peers[i].sin_addr, ipbuf, sizeof(ipbuf));
            uint16_t port = ntohs(peers[i].sin_port);
            std::cout << "TIME from " << ipbuf << ":" << port
                      << " | sync level=" << static_cast<int>(responses[i].synchronized)
                      << " | timestamp=" << responses[i].timestamp << " ms" << std::endl;
        }
    }

    // Close the UDP socket before exiting.
    if (close(sock) < 0) {
        syserr("close");
    }

    return EXIT_SUCCESS;
}

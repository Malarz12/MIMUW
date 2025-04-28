#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE   30
#define REPEAT_COUNT  30

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <multicast_colon_address> <local_port>", argv[0]);
    }

    const char *multicast_colon_address = argv[1];
    uint16_t port = read_port(argv[2]);

    // Create a socket.
    int socket_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Join the multicast group.
    struct ipv6_mreq ipv6_mreq;
    ipv6_mreq.ipv6mr_interface = 0;
    if (inet_pton(AF_INET6, multicast_colon_address, &ipv6_mreq.ipv6mr_multiaddr) <= 0) {
        fatal("inet_pton - invalid multicast address");
    }

    if (setsockopt(socket_fd, SOL_IPV6, IPV6_ADD_MEMBERSHIP, (void *) &ipv6_mreq, sizeof ipv6_mreq) < 0) {
        syserr("cannot join the multicast group");
    }

    // Bind the socket to a concrete address.
    struct sockaddr_in6 server_address;
    server_address.sin6_family = AF_INET6;  // IPv6
    server_address.sin6_addr = in6addr_any; // Listening on all interfaces.
    server_address.sin6_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        syserr("bind");
    }

    // Receive some messages.
    ssize_t received_length;
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        static char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        int flags = 0;
        struct sockaddr_in6 client_address;
        socklen_t address_length = (socklen_t) sizeof(client_address);

        received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, flags,
                                   (struct sockaddr *) &client_address, &address_length);
        if (received_length < 0) {
            syserr("recvfrom");
        }
        char client_ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &client_address.sin6_addr, client_ip, INET6_ADDRSTRLEN);
        uint16_t client_port = ntohs(client_address.sin6_port);
        printf("received %zd bytes from [%s]:%" PRIu16 ": %.*s\n",
               received_length, client_ip, client_port, (int) received_length, buffer);
    }

    // Leave the multicast group.
    if (setsockopt(socket_fd, SOL_IPV6, IPV6_DROP_MEMBERSHIP, (void *) &ipv6_mreq, sizeof(ipv6_mreq)) < 0) {
        syserr("cannot leave the multicast group");
    }
    close(socket_fd);
}

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
        fatal("usage: %s <multicast_dotted_address> <local_port>", argv[0]);
    }

    const char *multicast_dotted_address = argv[1];
    uint16_t port = read_port(argv[2]);

    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Join the multicast group.
    struct ip_mreq ip_mreq;
    ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (inet_aton(multicast_dotted_address, &ip_mreq.imr_multiaddr) == 0) {
        fatal("inet_aton - invalid multicast address\n");
    }

    if (setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &ip_mreq, sizeof ip_mreq) < 0) {
        syserr("cannot join the multicast group");
    }

    // Bind the socket to a concrete address.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        syserr("bind");
    }

    // Receive some messages.
    ssize_t received_length;
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        static char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        int flags = 0;
        struct sockaddr_in client_address;
        socklen_t address_length = (socklen_t) sizeof(client_address);

        received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, flags,
                                   (struct sockaddr *) &client_address, &address_length);
        if (received_length < 0) {
            syserr("recvfrom");
        }
        char const *client_ip = inet_ntoa(client_address.sin_addr);
        uint16_t client_port = ntohs(client_address.sin_port);
        printf("received %zd bytes from %s:%" PRIu16 ": %.*s\n",
               received_length, client_ip, client_port, (int) received_length, buffer);
    }

    // Leave the multicast group.
    if (setsockopt(socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *) &ip_mreq, sizeof(ip_mreq)) < 0) {
        syserr("cannot leave the multicast group");
    }
    close(socket_fd);
}

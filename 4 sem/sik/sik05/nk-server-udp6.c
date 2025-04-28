#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE 65600

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fatal("usage: %s <port>", argv[0]);
    }

    uint16_t port = read_port(argv[1]);

    // Create a socket.
    int socket_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Bind the socket to a concrete address IPv6.
    struct sockaddr_in6 server_address;
    server_address.sin6_family = AF_INET6;  // IPv6
    server_address.sin6_flowinfo = 0;
    server_address.sin6_addr = in6addr_any; // Listening on all interfaces.
    server_address.sin6_port = htons(port);
    server_address.sin6_scope_id = 0;

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }

    printf("listening on port %" PRIu16 "\n", port);

    ssize_t received_length;
    do {
        // Receive a message. Buffer should not be allocated on the stack.
        static char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer)); // Clean the buffer.

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
        printf("received %zd bytes from [%s]:%" PRIu16 "\n",
               received_length, client_ip, client_port);

    } while (received_length > 0);

    printf("exchange finished\n");

    close(socket_fd);
    return 0;
}

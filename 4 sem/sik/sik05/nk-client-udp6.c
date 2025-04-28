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

#define BUFFER_SIZE  20
#define ADDR_SIZE    40

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fatal("usage: %s <host> <port> <n> <k>", argv[0]);
    }

    char const *host = argv[1];
    uint16_t port = read_port(argv[2]);
    size_t number_of_messages = read_size(argv[3]); // n
    size_t message_length = read_size(argv[4]);     // k

    char *buffer = malloc(message_length);
    if (buffer == NULL) {
        syserr("malloc");
    }

    struct sockaddr_in6 server_address = get_server_address_ipv6(host, port);

    // Create a socket.
    int socket_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    for (size_t i = 0; i < number_of_messages; ++i) {
        memset(buffer, (int) i, message_length);
        int send_flags = 0;
        socklen_t address_length = (socklen_t) sizeof(server_address);
        ssize_t sent_length = sendto(socket_fd, buffer, message_length, send_flags,
                                     (struct sockaddr *) &server_address, address_length);
        if (sent_length < 0) {
            syserr("sendto");
        }
        else if ((size_t) sent_length != message_length) {
            fatal("incomplete sending");
        }

        printf("sent %zu bytes to [%s]:%" PRIu16 "\n",
               message_length, host, port);
    }

    free(buffer);
    close(socket_fd);
    return 0;
}

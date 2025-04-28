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
#include <time.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE   30
#define HOPS_VALUE    4
#define REPEAT_COUNT  3
#define SLEEP_TIME    1

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <remote_address> <remote_port>", argv[0]);
    }

    const char *remote_colon_address = argv[1];
    uint16_t remote_port = read_port(argv[2]);

    // Create a socket.
    int socket_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    int optval = HOPS_VALUE;
    if (setsockopt(socket_fd, SOL_IPV6, IPV6_MULTICAST_HOPS, (void *) &optval, sizeof optval) < 0) {
        syserr("cannot set HOPS");
    }

    struct sockaddr_in6 remote_address;
    remote_address.sin6_family = AF_INET6;
    remote_address.sin6_port = htons(remote_port);
    if (inet_pton(AF_INET6, remote_colon_address, &remote_address.sin6_addr) <= 0) {
        fatal("inet_pton - invalid multicast address");
    }

    // Announce the current time.
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        static char buffer[BUFFER_SIZE];
        time_t time_buffer;
        time(&time_buffer);
        strncpy(buffer, ctime(&time_buffer), BUFFER_SIZE - 1);
        size_t length = strnlen(buffer, BUFFER_SIZE) - 1;

        int send_flags = 0;
        socklen_t address_length = (socklen_t) sizeof(remote_address);
        ssize_t sent_length = sendto(socket_fd, buffer, length, send_flags,
                                     (struct sockaddr *) &remote_address, address_length);
        if (sent_length < 0) {
            syserr("sendto");
        }
        else if ((size_t) sent_length != length) {
            fatal("incomplete sending");
        }

        printf("sent %zu bytes to [%s]:%" PRIu16 "\n",
               length, remote_colon_address, remote_port);

        sleep(SLEEP_TIME);
    }

    close(socket_fd);
}

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

#define BUFFER_SIZE 200

static uint16_t read_port(char const *string) {
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(string, &endptr, 10);
    if (errno != 0 || *endptr != 0 || port == 0 || port > UINT16_MAX) {
        fatal("%s is not a valid port number", string);
    }
    return (uint16_t) port;
}

static struct sockaddr_in get_server_address(char const *host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);
    if (errcode != 0) {
        fatal("getaddrinfo: %s", gai_strerror(errcode));
    }

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET;   // IPv4
    send_address.sin_addr.s_addr =       // IP address
            ((struct sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr;
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fatal("usage: %s <host> <port> <number_of_packages> <size_of_packages> ...\n", argv[0]);
    }

    char const *host = argv[1];
    uint16_t port = read_port(argv[2]);
    uint16_t n = read_port(argv[3]);
    uint16_t k = read_port(argv[4]);

    struct sockaddr_in server_address = get_server_address(host, port);
    char const *server_ip = inet_ntoa(server_address.sin_addr);
    uint16_t server_port = ntohs(server_address.sin_port);
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    unsigned char *buffer = malloc(k);
    for (int i = 0; i < n; ++i) {
        sleep(10);
        unsigned char *buffer = malloc(k);
        if (buffer == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        memset(buffer, 0xAA, k);
        int send_flags = 0;
        socklen_t address_length = (socklen_t) sizeof(server_address);
        ssize_t sent_length = sendto(socket_fd, buffer, k, send_flags,
                                     (struct sockaddr *) &server_address, address_length);
        if (sent_length < 0) {
            syserr("sendto");
        }
        else if ((size_t) sent_length != k) {
            fatal("incomplete sending");
        }

        printf("sent to %s:%" PRIu16 ": '%s'\n", server_ip, server_port, argv[i]);
    }
    free(buffer);
    close(socket_fd);
    return 0;
}

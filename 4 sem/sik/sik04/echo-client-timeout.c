#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <host> <port>", argv[0]);
    }

    const char *host = argv[1];
    uint16_t port = read_port(argv[2]);
    struct sockaddr_in server_address = get_server_address(host, port);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    if (connect(socket_fd, (struct sockaddr *) &server_address,
                (socklen_t) sizeof(server_address)) < 0) {
        syserr("cannot connect to the server");
    }
    printf("connected to %s:%" PRIu16 "\n", host, port);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&timeout, sizeof(timeout))) {
        syserr("setsockopt");
    }

    static char line[BUFFER_SIZE];
    static char buffer[BUFFER_SIZE];
    memset(line, 0, BUFFER_SIZE);
    size_t to_send, send_from, total_sent, total_read;
    ssize_t len_sent, len_read;

    total_sent = 0;
    total_read = 0;
    for (;;) {
        if (fgets(line, BUFFER_SIZE - 1, stdin) == NULL) {
            break;
        }

        to_send = strlen(line);
        send_from = 0;
        while (to_send > 0) {
            len_sent = write(socket_fd, line + send_from, to_send);

            if (len_sent < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) { // przekroczenie czasu
                    len_sent = 0;
                }
                else {
                    syserr("write");
                }
            }
            else {
                to_send -= (size_t)len_sent;
                send_from += (size_t)len_sent;
                total_sent += (size_t)len_sent;
                printf("sent %zu bytes (%zu in total)\n", len_sent, total_sent);
            }

            if (len_sent > 0) {
                len_read = read(socket_fd, buffer, sizeof buffer);
                if (len_read < 0) {
                    syserr("read");
                }
                else if (len_read == 0) {
                    fatal("ending connection");
                }
                else {
                    total_read += (size_t)len_read;
                    printf("received %zu bytes (%zu in total): '%.*s'\n",
                           len_read, total_read, (int)len_read, buffer);
                }
            }
        }
    }
    shutdown(socket_fd, SHUT_RDWR);
}

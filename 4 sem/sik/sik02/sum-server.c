#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <endian.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sum-common.h"
#include "err.h"
#include "common.h"

#define QUEUE_LENGTH  5
#define SOCK_TIMEOUT  4

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fatal("usage: %s <port>", argv[0]);
    }

    uint16_t port = read_port(argv[1]);

    // Ignore SIGPIPE signals, so they are delivered as normal errors.
    signal(SIGPIPE, SIG_IGN);

    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Bind the socket to a concrete address.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        syserr("bind");
    }

    // Switch the socket to listening.
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        syserr("listen");
    }

    // Find out what port the server is actually listening on.
    socklen_t lenght = (socklen_t) sizeof server_address;
    if (getsockname(socket_fd, (struct sockaddr *) &server_address, &lenght) < 0) {
        syserr("getsockname");
    }

    printf("listening on port %" PRIu16 "\n", ntohs(server_address.sin_port));

    for (;;) {
        struct sockaddr_in client_address;

        int client_fd = accept(socket_fd, (struct sockaddr *) &client_address,
                               &((socklen_t) {sizeof(client_address)}));
        if (client_fd < 0) {
            syserr("accept");
        }

        char const *client_ip = inet_ntoa(client_address.sin_addr);
        uint16_t client_port = ntohs(client_address.sin_port);
        printf("accepted connection from %s:%" PRIu16 "\n", client_ip, client_port);

        // Set timeouts for the client socket.
        struct timeval to = {.tv_sec = SOCK_TIMEOUT, .tv_usec = 0};
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof to);
        setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof to);

        ssize_t read_length;
        ssize_t written_length;
        uint64_t sum = 0;
        for (;;) {
            data_pkt data;
            read_length = readn(client_fd, &data, sizeof data);
            if (read_length < 0) {
                if (errno == EAGAIN) {
                    printf("timeout\n");
                }
                else {
                    error("readn");
                }
                break;
            }
            else if (read_length == 0) {
                printf("connection closed\n");
                break;
            }
            else if ((size_t) read_length < sizeof data) {
                printf("connection closed without providing full data structure\n");
                break;
            }
 
            sum += (uint64_t) be32toh(data.number);

            printf("received %zd bytes from %s:%" PRIu16 "\n"
                   "received packet %" PRIu16 ": %" PRIu32 "\n"
                   "current sum: %" PRIu64 "\n",
                   read_length, client_ip, client_port,
                   be16toh(data.seq_no), be32toh(data.number),
                   sum);

            response_pkt resp;
            resp.sum = htobe64(sum);
            written_length = writen(client_fd, &resp, sizeof resp);
            if ((size_t) written_length < sizeof resp) {
                error("writen");
            }
            else {
                printf("reply sent\n");
            }
        }

        printf("finished serving %s:%" PRIu16 "\n", client_ip, client_port);
        close(client_fd);
    }
    close(socket_fd);
    return 0;
}

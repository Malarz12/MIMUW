#include <errno.h>
#include <endian.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE   1024
#define QUEUE_LENGTH     5
#define TIMEOUT       5000
#define CONNECTIONS      3

static bool finish = false;

/* Termination signal handling. */
static void catch_int(int sig) {
    finish = true;
    printf("signal %d catched so no new connections will be accepted\n", sig);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fatal("usage: %s <port>", argv[0]);
    }

    install_signal_handler(SIGINT, catch_int, SA_RESTART);

    uint16_t port = read_port(argv[1]);

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

    // Initialization of pollfd structures.
    struct pollfd poll_descriptors[CONNECTIONS];

    // The main socket has index 0.
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[0].events = POLLIN;

    for (int i = 1; i < CONNECTIONS; ++i) {
        poll_descriptors[i].fd = -1;
        poll_descriptors[i].events = POLLIN;
    }
    size_t active_clients = 0;
    static char buffer[BUFFER_SIZE];
    struct sockaddr_in client_address;

    do {
        for (int i = 0; i < CONNECTIONS; ++i) {
            poll_descriptors[i].revents = 0;
        }

        // After Ctrl-C the main socket is closed.
        if (finish && poll_descriptors[0].fd >= 0) {
            close(poll_descriptors[0].fd);
            poll_descriptors[0].fd = -1;
        }

        int poll_status = poll(poll_descriptors, CONNECTIONS, TIMEOUT);
        if (poll_status == -1 ) {
            if (errno == EINTR) {
                printf("interrupted system call\n");
            }
            else {
                syserr("poll");
            }
        }
        else if (poll_status > 0) {
            if (!finish && (poll_descriptors[0].revents & POLLIN)) {
                // New connection: new client is accepted.
                int client_fd = accept(poll_descriptors[0].fd,
                                       (struct sockaddr *) &client_address,
                                       &((socklen_t) {sizeof client_address}));
                if (client_fd < 0) {
                    syserr("accept");
                }

                // Searching for a free slot.
                bool accepted = false;
                for (int i = 1; i < CONNECTIONS; ++i) {
                    if (poll_descriptors[i].fd == -1) {
                        printf("received new connection (%d)\n", i);
                        poll_descriptors[i].fd = client_fd;
                        poll_descriptors[i].events = POLLIN;
                        active_clients++;
                        accepted = true;
                        break;
                    }
                }
                if (!accepted) {
                    close(client_fd);
                    printf("too many clients\n");
                }
                else {
                    char const *client_ip = inet_ntoa(client_address.sin_addr);
                    uint16_t client_port = ntohs(client_address.sin_port);
                    printf("accepted connection from %s:%" PRIu16 "\n",
                           client_ip, client_port);
                }
            }
            // Serve data connections.
            for (int i = 1; i < CONNECTIONS; ++i) {
                if (poll_descriptors[i].fd != -1 && (poll_descriptors[i].revents & (POLLIN | POLLERR))) {

                    ssize_t received_bytes = read(poll_descriptors[i].fd, buffer, BUFFER_SIZE);

                    if (received_bytes < 0) {
                        error("error when reading message from connection %d", i);
                        close(poll_descriptors[i].fd);
                        poll_descriptors[i].fd = -1;
                        active_clients--;
                    } else if (received_bytes == 0) {
                        printf("ending connection (%d)\n", i);
                        close(poll_descriptors[i].fd);
                        poll_descriptors[i].fd = -1;
                        active_clients--;
                    } else {
                        printf("received %zd bytes within connection (%d): '%.*s'\n",
                        received_bytes, i, (int) received_bytes, buffer);
                    }
                }
            }
        } else {
            printf("%d milliseconds passed without any events\n", TIMEOUT);
        }
    } while (!finish || active_clients > 0);

    if (poll_descriptors[0].fd >= 0) {
        close(poll_descriptors[0].fd);
    }
}

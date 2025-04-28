#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    printf("parent is listening on port %" PRIu16 "\n", ntohs(server_address.sin_port));

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
    struct sockaddr_in client_address;

    char buffer[CONNECTIONS][BUFFER_SIZE];
    ssize_t buffer_len[CONNECTIONS], buffer_pos[CONNECTIONS];

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
                error("interrupted system call");
            }
            else {
                syserr("poll");
            }
        }
        else if (poll_status > 0) {
            if (!finish && (poll_descriptors[0].revents & POLLIN)) {
                // New connection.
                int client_fd = accept(socket_fd, (struct sockaddr *) &client_address,
                                       &((socklen_t) {sizeof(client_address)}));
                if (client_fd < 0) {
                    syserr("accept");
                }

                // Set to nonblocking mode.
                if (fcntl(client_fd, F_SETFL, O_NONBLOCK)) {
                    syserr("fcntl");
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

                char const *client_ip = inet_ntoa(client_address.sin_addr);
                uint16_t client_port = ntohs(client_address.sin_port);
                printf("accepted connection from %s:%" PRIu16 "\n", client_ip, client_port);
            }

            // Existing connection.
            for (int i = 1; i < CONNECTIONS; ++i) {
                if (poll_descriptors[i].fd != -1) {
                    if ((poll_descriptors[i].revents & (POLLIN | POLLERR)) != 0) {
                        // Ready to read.
                        ssize_t received_bytes = read(poll_descriptors[i].fd, buffer[i], BUFFER_SIZE);

                        if (received_bytes < 0) {
                            error("error when reading message from connection %d (errno %d, %s)\n",
                                  i, errno, strerror(errno));
                            close(poll_descriptors[i].fd);
                            poll_descriptors[i].fd = -1;
                            active_clients -= 1;
                        } else if (received_bytes == 0) {
                            printf("ending connection (%d)\n", i);
                            close(poll_descriptors[i].fd);
                            poll_descriptors[i].fd = -1;
                            active_clients -= 1;
                        } else {
                            printf("received %zd bytes within connection (%d): '%.*s'\n",
                            received_bytes, i, (int) received_bytes, buffer[i]);
                            buffer_len[i] = received_bytes;
                            buffer_pos[i] = 0;
                            poll_descriptors[i].events = POLLOUT; // Switch from reading to writing.
                        }
                    }
                    if ((poll_descriptors[i].revents & POLLOUT) != 0) {
                        // Ready to write.
                        ssize_t sent_bytes = write(poll_descriptors[i].fd,
                                                   buffer[i] + buffer_pos[i],
                                                   buffer_len[i] - buffer_pos[i]);

                        if (sent_bytes < 0) {
                            error("error when writing message to connection %d (errno %d, %s)\n",
                                  i, errno, strerror(errno));
                            close(poll_descriptors[i].fd);
                            poll_descriptors[i].fd = -1;
                            active_clients -= 1;
                        }
                        else {
                            printf("sent %zd bytes within connection (%d)\n",
                                   sent_bytes, i);
                            buffer_pos[i] += sent_bytes;
                            if (buffer_pos[i] == buffer_len[i])
                                poll_descriptors[i].events = POLLIN; // Switch from writing to reading.
                        }
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

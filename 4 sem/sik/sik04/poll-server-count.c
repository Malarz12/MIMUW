#include <errno.h>
#include <endian.h>
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

#define BUFFER_SIZE      1024
#define QUEUE_LENGTH        5
#define TIMEOUT         5000
#define CLIENT_CONNECTIONS  3
#define TOTAL_FDS (2 + CLIENT_CONNECTIONS)

static bool finish = false;

static void catch_int(int sig) {
    finish = true;
    printf("signal %d catched so no new connections will be accepted\n", sig);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <client_port> <control_port>", argv[0]);
    }

    /* Instalacja obsługi SIGINT */
    install_signal_handler(SIGINT, catch_int, SA_RESTART);

    uint16_t client_port = read_port(argv[1]);
    uint16_t control_port = read_port(argv[2]);

    /* Utworzenie gniazda nasłuchującego dla klientów */
    int client_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_listen_fd < 0) {
        syserr("cannot create client socket");
    }

    struct sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = htonl(INADDR_ANY);
    client_address.sin_port = htons(client_port);

    if (bind(client_listen_fd, (struct sockaddr *) &client_address, (socklen_t) sizeof client_address) < 0) {
        syserr("bind client");
    }

    if (listen(client_listen_fd, QUEUE_LENGTH) < 0) {
        syserr("listen client");
    }

    int control_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (control_listen_fd < 0) {
        syserr("cannot create control socket");
    }

    struct sockaddr_in control_address;
    control_address.sin_family = AF_INET;
    control_address.sin_addr.s_addr = htonl(INADDR_ANY);
    control_address.sin_port = htons(control_port);

    if (bind(control_listen_fd, (struct sockaddr *) &control_address, (socklen_t) sizeof control_address) < 0) {
        syserr("bind control");
    }

    if (listen(control_listen_fd, QUEUE_LENGTH) < 0) {
        syserr("listen control");
    }

    printf("listening on client port %" PRIu16 "\n", ntohs(client_address.sin_port));
    printf("listening on control port %" PRIu16 "\n", ntohs(control_address.sin_port));
    struct pollfd poll_descriptors[TOTAL_FDS];
    poll_descriptors[0].fd = client_listen_fd;
    poll_descriptors[0].events = POLLIN;

    poll_descriptors[1].fd = control_listen_fd;
    poll_descriptors[1].events = POLLIN;

    for (int i = 2; i < TOTAL_FDS; ++i) {
        poll_descriptors[i].fd = -1;
        poll_descriptors[i].events = POLLIN;
    }

    size_t active_clients = 0;
    size_t total_clients = 0;
    static char buffer[BUFFER_SIZE];

    do {
        for (int i = 0; i < TOTAL_FDS; ++i) {
            poll_descriptors[i].revents = 0;
        }

        if (finish) {
            if (poll_descriptors[0].fd != -1) {
                close(poll_descriptors[0].fd);
                poll_descriptors[0].fd = -1;
            }
            if (poll_descriptors[1].fd != -1) {
                close(poll_descriptors[1].fd);
                poll_descriptors[1].fd = -1;
            }
        }

        int poll_status = poll(poll_descriptors, TOTAL_FDS, TIMEOUT);
        if (poll_status == -1 ) {
            if (errno == EINTR) {
                printf("interrupted system call\n");
            }
            else {
                syserr("poll");
            }
        }
        else if (poll_status > 0) {

            /* 1. Obsługa nowego połączenia klientów (indeks 0) */
            if (!finish && (poll_descriptors[0].revents & POLLIN)) {
                struct sockaddr_in cli_addr;
                socklen_t cli_len = sizeof(cli_addr);
                int client_fd = accept(poll_descriptors[0].fd, (struct sockaddr *) &cli_addr, &cli_len);
                if (client_fd < 0) {
                    syserr("accept client");
                }

                bool accepted = false;
                /* Szukamy wolnego miejsca w tablicy dla połączenia klienta.
                   Szukamy w indeksach od 2 do TOTAL_FDS-1. */
                for (int i = 2; i < TOTAL_FDS; ++i) {
                    if (poll_descriptors[i].fd == -1) {
                        poll_descriptors[i].fd = client_fd;
                        poll_descriptors[i].events = POLLIN;
                        active_clients++;
                        total_clients++;
                        accepted = true;
                        printf("received new client connection (slot %d)\n", i);
                        char const *cli_ip = inet_ntoa(cli_addr.sin_addr);
                        uint16_t cli_port = ntohs(cli_addr.sin_port);
                        printf("accepted client from %s:%" PRIu16 "\n", cli_ip, cli_port);
                        break;
                    }
                }
                if (!accepted) {
                    close(client_fd);
                    printf("too many clients\n");
                }
            }

            if (!finish && (poll_descriptors[1].revents & POLLIN)) {
                struct sockaddr_in ctrl_addr;
                socklen_t ctrl_len = sizeof(ctrl_addr);
                int control_fd = accept(poll_descriptors[1].fd,
                                        (struct sockaddr *) &ctrl_addr, &ctrl_len);
                if (control_fd < 0) {
                    syserr("accept control");
                }

                char ctrl_buffer[BUFFER_SIZE];
                ssize_t cmd_len = read(control_fd, ctrl_buffer, BUFFER_SIZE - 1);
                if (cmd_len > 0) {
                    ctrl_buffer[cmd_len] = '\0';
                    if (strncmp(ctrl_buffer, "count", 5) == 0) {
                        char reply[128];
                        snprintf(reply, sizeof(reply),
                                 "number of active clients: %zu\ntotal number of clients: %zu\n",
                                 active_clients, total_clients);
                        write(control_fd, reply, strlen(reply));
                    }
                }
                close(control_fd);
            }

            for (int i = 2; i < TOTAL_FDS; ++i) {
                if (poll_descriptors[i].fd != -1 &&
                    (poll_descriptors[i].revents & (POLLIN | POLLERR))) {

                    ssize_t received_bytes = read(poll_descriptors[i].fd, buffer, BUFFER_SIZE);
                    if (received_bytes < 0) {
                        error("error when reading message from client on slot %d", i);
                        close(poll_descriptors[i].fd);
                        poll_descriptors[i].fd = -1;
                        active_clients--;
                    } else if (received_bytes == 0) {
                        printf("ending client connection on slot %d\n", i);
                        close(poll_descriptors[i].fd);
                        poll_descriptors[i].fd = -1;
                        active_clients--;
                    } else {
                        printf("received %zd bytes from client on slot %d: '%.*s'\n",
                               received_bytes, i, (int) received_bytes, buffer);
                    }
                }
            }
        } else {
            printf("%d milliseconds passed without any events\n", TIMEOUT);
        }
    } while (!finish || active_clients > 0);

    if (poll_descriptors[0].fd != -1)
        close(poll_descriptors[0].fd);
    if (poll_descriptors[1].fd != -1)
        close(poll_descriptors[1].fd);

    return 0;
}

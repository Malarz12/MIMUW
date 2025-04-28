#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <endian.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <wait.h>

#include "err.h"
#include "common.h"

#define QUEUE_LENGTH     5
#define BUFFER_SIZE      1024
#define NFORKS           4

static pid_t pids[NFORKS];

static void handle_signal(int signal) {
    fprintf(stderr, "Signal %d catched.\n", signal);

    // Stop all children.
    for (int i = 0; i < NFORKS; i++) {
        kill(pids[i], SIGTERM);
    }
    while (wait(NULL) > 0);
}

static pid_t make_child(int socket_fd) {
    pid_t pid = fork();
    if (pid < 0) {
        syserr("fork");
    }
    else if (pid > 0) { // parent
        return pid;
    }

    int pid_int = getpid();
    printf("[%d] child process is starting \n", pid_int);

    for (;;) {
        struct sockaddr_in client_address;
        int client_fd = accept(socket_fd, (struct sockaddr *) &client_address,
                               &((socklen_t) {sizeof(client_address)}));
        if (client_fd < 0) {
            syserr("accept");
        }

        char const *client_ip = inet_ntoa(client_address.sin_addr);
        uint16_t client_port = ntohs(client_address.sin_port);
        printf("[%d] accepted connection from %s:%" PRIu16 "\n", pid_int, client_ip, client_port);

        static char buf[BUFFER_SIZE];
        ssize_t len;
        do {
            memset(buf, 0, sizeof(buf));
            len = read(client_fd, buf, sizeof(buf));
            if (len < 0)
                syserr("read");
            else if (len == 0)
                printf("[%d] closing connection\n", pid_int);
            else
                printf("[%d]-->%.*s\n", pid_int, (int) len, buf);
        } while (len > 0);
        close(client_fd);
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fatal("usage: %s <port>", argv[0]);
    }

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
    printf("server is listening on port %" PRIu16 "\n", ntohs(server_address.sin_port));

    for (int i = 0; i < NFORKS; i++) {
        pids[i] = make_child(socket_fd);
    }

    install_signal_handler(SIGINT, handle_signal, 0);

    // Wait for children.
    while (wait(NULL) > 0);
    close(socket_fd);
    exit(0);
}

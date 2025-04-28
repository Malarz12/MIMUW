/*
 Program klienta uruchamiamy z dwoma parametrami: nazwą lub adresem serwera
 i numerem jego portu. Klient łączy się z serwerem, po czym wczytuje
 linie tekstu i wysyła je do serwera.  Wpisanie "exit" kończy pracę.
*/

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE 1024

static const char exit_string[] = "exit";

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

    static char line[BUFFER_SIZE];
    memset(line, 0, BUFFER_SIZE);

    do {
        if (fgets(line, BUFFER_SIZE - 1, stdin) == NULL) {
            break;
        }
        size_t data_to_send = strnlen(line, BUFFER_SIZE);
        ssize_t written_length = writen(socket_fd, line, data_to_send);
        if (written_length < 0) {
            syserr("writen");
        }
        else if ((size_t) written_length != data_to_send) {
            fatal("incomplete writen");
        }

    } while (strncmp(line, exit_string, sizeof exit_string - 1) != 0);

    close(socket_fd);
    return 0;
}

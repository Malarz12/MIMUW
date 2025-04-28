#include <endian.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "sum-common.h"
#include "err.h"
#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <host> <port>\n", argv[0]);
    }

    char const *host = argv[1];
    uint16_t port = read_port(argv[2]);
    struct sockaddr_in server_address = get_server_address(host, port);

    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Connect to the server.
    if (connect(socket_fd, (struct sockaddr *) &server_address,
                (socklen_t) sizeof(server_address)) < 0) {
        syserr("cannot connect to the server");
    }

    data_pkt data_to_send;
    uint16_t seq_no = 0;
    uint32_t number = 0;

    // Read numbers from the standard input.
    while (scanf("%" SCNu32, &number) == 1) {
        printf("sending number %" PRIu32 "...\n", number);

        // Send numbers in network byte order
        data_to_send.seq_no = htobe16(seq_no);
        data_to_send.number = htobe32(number);
        seq_no++;

        ssize_t written_length = writen(socket_fd, &data_to_send, sizeof data_to_send);
        if (written_length < 0) {
            syserr("writen");
        }
        // Here we can convert the type, because we know, that written_length >= 0.
        else if ((size_t) written_length != sizeof data_to_send) {
            fatal("incomplete writen");
        }

        response_pkt resp;
        ssize_t read_length = readn(socket_fd, &resp, sizeof resp);
        if (read_length < 0) {
            syserr("readn");
        }
        else if (read_length == 0) {
            error("connection closed");
            break;
        }
        // Here we can convert the type, because we know, that read_length > 0.
        else if ((size_t) read_length < sizeof resp) {
            fatal("incomplete readn");
        }

        printf("current sum: %" PRIu64 "\n", be64toh(resp.sum));
    }

    close(socket_fd);
    return 0;
}

#ifndef SUM_COMMON_H
#define SUM_COMMON_H

#include <inttypes.h>
#include <stddef.h>
#include <sys/types.h>

// 1) Send uint16_t, int32_t etc., not int.
//    The length of int is platform-dependent.
// 2) If we want to send a structure, we have to declare it
//    with __attribute__((__packed__)). Otherwise the compiler
//    may add a padding bewteen fields. In the following example
//    sizeof (data_pkt) is then 8, not 6.

typedef struct __attribute__((__packed__)) {
    uint16_t seq_no;
    uint32_t number;
} data_pkt;

typedef struct __attribute__((__packed__)) {
    uint64_t sum;
} response_pkt;

ssize_t	readn(int fd, void *vptr, size_t n);
ssize_t	writen(int fd, const void *vptr, size_t n);

#endif // SUM_COMMON_H

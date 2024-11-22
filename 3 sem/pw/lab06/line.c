#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"

#define BUFFER_SIZE 32

int main(int argc, char* argv[]) {
    int n_children = 0;
    if (argc > 2)
        fatal("");
    if (argc == 2)
        n_children = atoi(argv[1]);

    if (n_children <= 0) {
        printf("Process %d \n", getpid());
        return 0;
    }
    pid_t pid;
    printf("Process %d. %d left\n", getpid(), n_children);
    ASSERT_SYS_OK(pid = fork());

    if(pid != 0) {
        ASSERT_SYS_OK(wait(NULL));
    }
    else if (pid == 0) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, sizeof(buffer), "%d", n_children - 1);
        ASSERT_SYS_OK(execl(argv[0], argv[0], buffer, NULL));
        fatal("");
    }

    return 0;
}

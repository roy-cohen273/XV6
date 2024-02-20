#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_NUMBER 35

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // TODO: finish writing this
        close(p[1]);
        exit(0);
    }

    close(p[0]);
    for (int i = 2; i <= MAX_NUMBER; i++) {
        write(p[1], &i, sizeof i);
    }
    close(p[1]);
    wait(0);

    exit(0);
}
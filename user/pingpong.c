#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p1[2];
    pipe(p1);
    int p2[2];
    pipe(p2);

    if (fork() == 0) {
        // child process
        int pid = getpid();
        char c;
        read(p1[0], &c, 1);
        printf("%d: received ping\n", pid);
        write(p2[1], &c, 1);
        exit(0);
    }

    int pid = getpid();
    char c = 0x27; // my favorite byte
    write(p1[1], &c, 1);
    read(p2[0], &c, 1);
    printf("%d: received pong\n", pid);

    exit(0);
}
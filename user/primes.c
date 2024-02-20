#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_NUMBER 35

/**
 * Generates a pipe and stores the read & write ends of the pipe in `pread` & `pwrite`.
*/
int
generate_pipe(int *pread, int *pwrite) {
    int p[2];
    int result = pipe(p);
    *pread = p[0];
    *pwrite = p[1];
    return result;
}

/**
 * code for child process, given the pipe to read from.
*/
void
child_main(int pread)
{
    int n;
    if (!read(pread, &n, sizeof n)) {
        close(pread);
        exit(0);
    }
    printf("prime %d\n", n);
    int new_pread, new_pwrite;
    generate_pipe(&new_pread, &new_pwrite);
    if (fork() == 0) {
        close(pread);
        close(new_pwrite);
        child_main(new_pread);
    }
    close(new_pread);
    int i;
    while (read(pread, &i, sizeof i)) {
        if (i % n != 0) {
            write(new_pwrite, &i, sizeof i);
        }
    }
    close(new_pwrite);
    close(pread);
    wait(0);
    exit(0);
}

int
main(int argc, char *argv[])
{
    int pread, pwrite;
    generate_pipe(&pread, &pwrite);

    if (fork() == 0) {
        close(pwrite);
        child_main(pread);
    }

    close(pread);
    for (int i = 2; i <= MAX_NUMBER; i++) {
        write(pwrite, &i, sizeof i);
    }
    close(pwrite);
    wait(0);
    exit(0);
}
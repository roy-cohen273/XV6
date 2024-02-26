#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLINE 512 // max length of input line

int
main(int argc, char *argv[])
{
    char *child_argv[MAXARG];
    memmove(child_argv, argv + 1, (argc - 1) * sizeof(*argv));

    char buf[MAXLINE];
    char *p = buf;
    while (read(0, p++, 1) == 1) { // TODO: fix buffer overflow
        if (p[-1] == '\n') {
            char **next_arg = child_argv + argc - 1;
            *next_arg++ = buf;
            for (char *q = buf; q < p; q++) {
                if (*q == ' ' || *q == '\n') {
                    *q = '\0';
                    *next_arg++ = q + 1;
                }
            }
            next_arg--;
            *next_arg = 0;
            if (fork() == 0) {
                exec(argv[1], child_argv);
            }
            wait(0);
            p = buf;
        }
    }
    exit(0);
}
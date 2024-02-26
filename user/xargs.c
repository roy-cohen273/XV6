#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#include <stdbool.h>

#define MAXLINE 512 // max length of input line

int
main(int argc, char *argv[])
{
    char *child_argv[MAXARG];
    memmove(child_argv, argv + 1, (argc - 1) * sizeof(*argv));

    char buf[MAXLINE];
    char *arg_start = buf;
    char *p = buf;
    char *buf_end = buf + (sizeof buf);
    int arg = argc - 1;
    bool end_of_line = false;
    while (1) {
        if (p >= buf_end) {
            fprintf(2, "find: input line too long\n");
            exit(0);
        }
        if (arg >= MAXARG) {
            fprintf(2, "find: too many arguments\n");
            exit(0);
        }
        if (read(0, p++, 1) != 1) {
            break;
        }
        if (p[-1] == ' ' || p[-1] == '\n') {
            if (p[-1] == '\n') {
                end_of_line = true;
            }
            p[-1] = '\0';
            child_argv[arg++] = arg_start;
            arg_start = p;
        }
        if (end_of_line) {
            child_argv[arg] = 0;
            if (fork() == 0) {
                exec(argv[1], child_argv);
            }
            wait(0);
            arg_start = buf;
            p = buf;
            arg = argc - 1;
            end_of_line = false;
        }
    }
    exit(0);
}

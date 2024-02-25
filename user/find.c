#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *
path_concat(const char *s1, const char *s2)
{
    // returns "$(s1)/$(s2)"

    char *result = malloc(strlen(s1) + strlen(s2) + 2);
    if (result == 0) {
        return 0;
    }
    char *p = result;
    while (*s1 != '\0') {
        *p++ = *s1++;
    }
    *p++ = '/';
    while (*s2 != '\0') {
        *p++ = *s2++;
    }
    *p = '\0';
    return result;
}

void
find(char *path, char *name)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_DEVICE:
        case T_FILE:
            // check if `path` ends with "/`name`"
            if (strlen(name) + 1 > strlen(path)) {
                break;
            }
            if (path[strlen(path) - (strlen(name) + 1)] != '/') {
                break;
            }
            if (strcmp(path + (strlen(path) - strlen(name)), name) != 0) {
                break;
            }
            // `path` does end with "/`name`"
            printf("%s\n", path);
            break;
            
        case T_DIR:
            struct dirent de;
            while (read(fd, &de, sizeof de) == sizeof de) {
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0 || strcmp(de.name, "") == 0) {
                    continue;
                }
                char *new_path = path_concat(path, de.name);
                if (new_path == 0) {
                    fprintf(2, "find: path too long %s/%s", path, de.name);
                    continue;
                }
                find(new_path, name);
            }
            break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "USAGE: %s <directory> <file name>\n", argv[0]);
        exit(0);
    }

    find(argv[1], argv[2]);
    exit(0);
}
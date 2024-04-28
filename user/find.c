#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *pattern) {
    char buf[512];
    struct dirent de;
    struct stat st;

    int fd;
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: can not open %s\n", path);
        exit(-1);
    }

    if (fstat(fd, &st)) {
        fprintf(2, "find: can not stat %s\n", path);
        exit(-1);
    }

    if (st.type == T_FILE) {
        char *p;
        for (p = path + strlen(path); p > path && *(p - 1) != '/'; --p);

        if (!strcmp(p, pattern)) {
            printf("%s\n", path);
        }
    } else if (st.type == T_DIR) {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            fprintf(2, "find: path too long\n");
            exit(-1);
        }
        strcpy(buf, path);
        char *p = buf + strlen(buf);
        *(p++) = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (!de.inum || !strcmp(de.name, ".") || !strcmp(de.name, "..")) {
                continue;
            }
            strcpy(p, de.name);
            p[strlen(de.name)] = 0;
            find(buf, pattern);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find path name\n");
        exit(-1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
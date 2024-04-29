#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

struct reader {
    char buf[512], *ptr;
    int len;
};

struct reader *reader() {
    struct reader *rd = malloc(sizeof(struct reader));
    rd->ptr = rd->buf;
    rd->len = 0;
    return rd;
}

int getchar(struct reader *rd) {
    if (rd->ptr == rd->buf + rd->len) {
        rd->ptr = rd->buf;
        rd->len = read(0, rd->buf, sizeof(rd->buf));
    }
    if (rd->len == 0) {
        return 0;
    }
    return *(rd->ptr++);
}

void parse_arguments(char **begin, char **end, char *s) {
    while (*s) {
        if (*s == ' ') {
            ++s;
            continue;
        }

        if (begin == end) {
            fprintf(2, "xargs: too many arguments\n");
            exit(-1);
        }
        char *t = s;
        while (*t && *t != ' ') {
            ++t;
        }

        *begin = malloc((t - s + 1) * sizeof(char));
        memcpy(*begin, s, t - s);
        (*begin)[t - s] = 0;

        ++begin;

        s = t;
    }

    if (begin == end) {
        fprintf(2, "xargs: too many arguments\n");
        exit(-1);
    }
    *begin = (char *)0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(2, "Usage: xargs command [args]\n");
        exit(-1);
    }

    struct reader *rd = reader();

    for (char c = getchar(rd); c; c = getchar(rd)) {
        char s[512], *p;
        for (p = s; c && c != '\n'; *(p++) = c, c = getchar(rd));
        *p = 0;

        if (fork() > 0) {
            wait((int *)0);
        } else {
            char *my_argv[MAXARG + 1];
            memcpy(my_argv, argv + 1, (argc - 1) * sizeof(char *));
            parse_arguments(my_argv + argc - 1, my_argv + MAXARG + 1, s);

            if (exec(my_argv[0], my_argv)) {
                fprintf(2, "fail to exec ");
                for (char **p = my_argv; *p; ++p) {
                    fprintf(2, "%s ", *p);
                }
                fprintf(2, "\n");
            }
            exit(0);
        }
    }

    free(rd);
    exit(0);
}
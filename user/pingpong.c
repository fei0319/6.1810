#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }

    int p[2];
    pipe(p);

    int pid = fork();
    if (pid > 0) {
        char buf[] = {'a'};

        write(p[1], buf, 1);
        close(p[1]);

        wait((int *)0);

        read(p[0], buf, 1);
        close(p[0]);

        printf("%d: received pong\n", getpid());
    } else {
        char buf[1];

        read(p[0], buf, 1);
        close(p[0]);

        printf("%d: received ping\n", getpid());

        write(p[1], buf, 1);
        close(p[1]);
    }
    exit(0);
}
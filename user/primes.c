#include "kernel/types.h"
#include "user/user.h"

int is_prime(int x) {
    if (x == 0) {
        return 0;
    }
    for (int i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(2, "Usage: primes\n");
        exit(1);
    }

    int p[2];
    pipe(p);

    if (fork() > 0) {
        close(p[0]);
        for (int i = 2; i <= 35; ++i) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
    } else {
        for (;;) {
            close(p[1]);

            int x = 0;
            while (!is_prime(x)) {
                int c = read(p[0], &x, sizeof(int));
                if (c == 0) {
                    break;
                }
            }

            int p1[2];
            pipe(p1);
            char buf[10];
            while (write(p1[1], buf, read(p[0], buf, sizeof(buf))));
            close(p[0]);
            memcpy(p, p1, sizeof(p1));

            if (is_prime(x)) {
                printf("prime %d\n", x);
            }
            if (!is_prime(x) || fork() > 0) {
                close(p[0]);
                close(p[1]);
                break;
            }
        }
    }
    wait((int *)0);
    exit(0);
}
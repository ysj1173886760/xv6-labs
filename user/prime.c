#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void work(int p[2]) {
    // close the pipe to parent, because we don't need to write to parent
    close(p[1]);
    char buf[4];

    if (read(p[0], buf, 4) != 4) {
        fprintf(2, "failed to read from parent process\n");
        exit(1);
    }

    int cur_prime = *((int *)buf);
    int p2c[2];
    // 0 indicate that we havn't create child process yet
    int flag = 0;
    printf("prime %d\n", cur_prime);

    while (read(p[0], buf, 4) == 4) {
        int n = *((int *)buf);
        if (n % cur_prime != 0) {
            if (flag == 0) {
                pipe(p2c);
                if (fork() == 0) {
                    work(p2c);
                }
                close(p2c[0]);
                flag = 1;
            }
            write(p2c[1], &n, 4);
        }
    }

    // close read from parent
    close(p[0]);
    // if we have child process, then close write to child
    close(p2c[1]);

    // wait for all child exit
    while (wait(0) != -1) ;
    exit(0);
}

int
main (int argc, char *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0) {
        /* child process */
        work(p);
    } else {
        close(p[0]);
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, 4);
        }
        close(p[1]);
        wait(0);
    }

    exit(0);
}
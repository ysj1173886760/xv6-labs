#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFSIZE 512

char buf[BUFSIZE];
int readline(char *new_argv[MAXARG], int curr_argc) {
    int n = 0;
    while (read(0, buf + n, 1)) {
        if (buf[n] == '\n') {
            break;
        }
        n++;
        if (n >= BUFSIZE) {
            fprintf(2, "argument is too long\n");
            exit(1);
        }
    }
    buf[n] = 0;

    if (n == 0) {
        return 0;
    }

    int ptr = 0;
    while (ptr < n) {
        new_argv[curr_argc++] = buf + ptr;
        while (ptr < n && buf[ptr] != ' ') {
            ptr++;
        }
        while (ptr < n && buf[ptr] == ' ') {
            buf[ptr++] = 0;
        }
        // printf("xargs: %s\n", new_argv[curr_argc - 1]);
    }
    return curr_argc;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "useage: xargs command [arg]\n");
        exit(1);
    }

    char *command = malloc(strlen(argv[1] + 1));
    strcpy(command, argv[1]);

    char *new_argv[MAXARG];
    for (int i = 1; i < argc; i++) {
        new_argv[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(new_argv[i - 1], argv[i]);
    }
    int curr_argc;
    while ((curr_argc = readline(new_argv, argc - 1)) != 0) {
        new_argv[curr_argc] = 0;
        if (fork() == 0) {
            exec(command, new_argv);
            fprintf(2, "xargs: exec %s failed\n", command);
            exit(1);
        }

        wait(0);
    }

    exit(0);
}
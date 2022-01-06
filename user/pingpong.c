#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main (int argc, char *argv[])
{
  int p2c[2], c2p[2];
  pipe(p2c);
  pipe(c2p);
  char buffer[10];
  if (fork() != 0) {
    close(p2c[0]);
    close(c2p[1]);
    write(p2c[1], "a", 1);
    read(c2p[0], buffer, 1);
    printf("%d: received pong\n", getpid());
    close(p2c[1]);
    close(c2p[0]);
  } else {
    close(p2c[1]);
    close(c2p[0]);
    read(p2c[0], buffer, 1);
    printf("%d: received ping\n", getpid());
    write(c2p[1], "a", 1);
    close(p2c[0]);
    close(c2p[1]);
  }

  exit(0);
}
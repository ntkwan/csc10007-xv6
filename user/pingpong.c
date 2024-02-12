#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc != 1)
  {
    fprintf(2, "usage: pingpong\n");
    exit(1);
  }
  int p[2];
  char buf;
  pipe(p);
  if (fork() == 0)
  {
    read(p[0], &buf, 1);
    fprintf(1, "%d: received ping\n", getpid());
    write(p[1], &buf, 1);
  }
  else
  {
    write(p[1], &buf, 1);
    read(p[0], &buf, 1);
    fprintf(1, "%d: received pong\n", getpid());
  }
  exit(0);
}
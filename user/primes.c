#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int p[2])
{
  int prime, n;
  close(p[1]);
  if (read(p[0], &prime, sizeof(prime)) != 4)
    exit(1);
  printf("prime %d\n", prime);

  if (read(p[0], &n, sizeof(prime)) == 4)
  {
    int fd[2];
    pipe(fd);
    if (fork() != 0)
    {
      close(fd[0]);
      if (n % prime)
        write(fd[1], &n, sizeof(n));
      while (read(p[0], &n, sizeof(n)) == 4)
        if (n % prime)
          write(fd[1], &n, sizeof(n));
      close(fd[1]);
      close(p[0]);
      wait(0);
    }
    else
      sieve(fd);
  }
}

int main(int argc, char *argv[])
{
  if (argc != 1)
  {
    fprintf(2, "usage: primes\n");
    exit(1);
  }
  int p[2];
  pipe(p);
  if (fork() == 0)
    sieve(p);
  else
  {
    close(p[0]);
    for (int i = 2; i <= 35; i++)
      write(p[1], &i, sizeof(i));
    close(p[1]);
    wait(0);
  }
  exit(0);
}
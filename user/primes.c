#include "kernel/types.h"
#include "user/user.h"

void
sendPrime(int readfd)
{
  int prime;
  if(!read(readfd, &prime, sizeof(prime))) {
    exit(0);
  }
  printf("prime %d\n", prime);
  

  int pipefd[2];
  if (pipe(pipefd) < 0) {
    fprintf(2, "pipe error\n");
    exit(1);
  }

  int cpid = fork();
  if (cpid < 0) {
    fprintf(2, "fork error\n");
    exit(1);
  }
  if (cpid == 0) { // child
    close(pipefd[1]);
    close(readfd);
    sendPrime(pipefd[0]);
    close(pipefd[0]);
  }
  else { // parent 
    close(pipefd[0]);
    int num;
    while (read(readfd, &num, sizeof(num)) == sizeof(num)) {
      if (num % prime != 0) {
        if (write(pipefd[1], &num, sizeof(num)) != sizeof(num)) {
          fprintf(2, "parent write error\n");
          exit(1);
        }
      }
    }
    close(pipefd[1]);
    wait(0);
  }
}

int
main(int argc, char *argv[])
{
  int pipefd[2];

  if (pipe(pipefd) < 0) {
    fprintf(2, "pipe error\n");
    exit(1);
  }

  int cpid = fork();
  if (cpid < 0) {
    fprintf(2, "fork error\n");
    exit(1);
  }

  if (cpid == 0) { // child
    close(pipefd[1]);
    sendPrime(pipefd[0]);
    close(pipefd[0]);
  }
  else { // parent
    close(pipefd[0]);

    for (int num = 2; num <= 280; num++) {
      if (write(pipefd[1], &num, sizeof(num)) != sizeof(num)) {
        fprintf(2, "parent write error\n");
        exit(1);
      }
    }
    close(pipefd[1]);
    wait(0);
  }
  exit(0);
}

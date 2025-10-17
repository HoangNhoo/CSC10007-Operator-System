#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pipefd[2];
  int pipefd2[2];

  if (pipe(pipefd) < 0 || pipe(pipefd2) < 0) {
    fprintf(2, "pipe error\n");
    exit(1);
  }

  int cpid = fork();
  if (cpid == 0) { // child
    close(pipefd[1]);
    close(pipefd2[0]);

    char buf;
    if (read(pipefd[0], &buf, 1) != 1) {
      fprintf(2, "child read error\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    if (write(pipefd2[1], "b", 1) != 1) {
      fprintf(2, "child write error\n");
      exit(1);
    }

    close(pipefd[0]);
    close(pipefd2[1]);
  } else { //parent
    close(pipefd[0]);
    close(pipefd2[1]);

    if (write(pipefd[1], "a", 1) != 1) {
      fprintf(2, "parent write error\n");
      exit(1);
    }

    char buf;
    if (read(pipefd2[0], &buf, 1) != 1) {
      fprintf(2, "parent read error\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(pipefd[1]);
    close(pipefd2[0]);

    wait(0);
  }
  exit(0);
}

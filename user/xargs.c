#include "kernel/types.h"
#include "user/user.h"

void
xargs(char* cmd, char** args, int n)
{
  char* argv[22];
  int i = 0;
  while(args[i])
  {
    argv[i] = args[i];
    i++;
  }
}

int
main(int argc, char* argv[])
{
  if(argc < 2)
  {
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }
  //check if command or -n is given
  char* cmd = argv[1];
  int n = 0;
  if(strcmp(cmd, "-n") == 0)
  {
    if(argc < 4)
    {
      fprintf(2, "Usage: xargs -n number command [args...]\n");
      exit(1);
    }
    n = atoi(argv[2]);
    cmd = argv[3];
  }
  if(n < 0 || n + argc - (n > 0 ? 4 : 2) > 20)
  {
    fprintf(2, "xargs: invalid number %d (max n : 20)\n", n);
    exit(1);
  }
  xargs(cmd, &argv[1 + (n > 0 ? 3 : 1)], n);
  exit(0);
}

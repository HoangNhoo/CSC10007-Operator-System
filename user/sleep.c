#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
  if(argc != 2)
  {
    write(1, "sleep: missing operand\n", 23);
    exit(1);
  }
  int time = atoi(argv[1]);
  int seconds = time * 10;
  if(seconds < 0)
  {
    write(1, "sleep: invalid time interval\n", 30);
    exit(1);
  }
  sleep(seconds);
  exit(0);
}

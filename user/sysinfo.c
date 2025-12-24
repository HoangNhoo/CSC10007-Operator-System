#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

void
sinfo(struct sysinfo* info)
{
  if(sysinfo(info) < 0)
  {
    printf("FAIL: sysinfo failed\n");
    exit(1);
  }
}

void
test()
{
  struct sysinfo info;
  sinfo(&info);

  printf("freemem: %ld bytes\n", info.freemem);
  printf("nproc:   %ld processes\n", info.nproc);
  printf("loadavg: %ld (scaled by 100)\n", info.loadavg);
}

int
main(int argc, char *argv[])
{
  test();
  exit(0);
}

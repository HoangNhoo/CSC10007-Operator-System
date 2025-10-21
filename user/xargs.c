#include "kernel/types.h"
#include "user/user.h"

void
xargs(char* cmd, char** args)
{
  int curr_argc = 0, new_argc = 0;
  while(args[curr_argc++])
    ;
  curr_argc--;
  char buf[512];
  char c;
  char **new_argv = malloc((curr_argc + 20) * sizeof(char *));
  for(int i = 0; i < curr_argc; i++)
    new_argv[i] = args[i];
  int i = 0;
  new_argc = curr_argc;
  while(read(0, &c, 1) > 0)
  {
    if(c == '\n')
    {
      if(i > 0)
      {
        buf[i] = '\0';
        new_argv[new_argc] = malloc(strlen(buf) + 1);
        strcpy(new_argv[new_argc++], buf);
        i = 0;
      }
      new_argv[new_argc] = 0;
      int cid = fork();
      if(cid < 0)
      {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
      }
      if(cid == 0)
      {
        exec(cmd, new_argv);
        for(int j = curr_argc; j < new_argc; j++)
          free(new_argv[j]);
        free(new_argv);
        exit(0);
      }
      else
      {
        wait(0);
      }
      for(int j = curr_argc; j < new_argc; j++)
        free(new_argv[j]);
      new_argc = curr_argc;
    }
    else {
      if(c == ' ' || c == '\t')
      {
        if(i == 0)
          continue;
        buf[i] = '\0';
        new_argv[new_argc] = malloc(strlen(buf) + 1);
        strcpy(new_argv[new_argc++], buf);
        i = 0;
      }
      else
      {
        if (i >= sizeof(buf) - 1) {
          fprintf(2, "xargs: argument too long\n");
          for (int j = curr_argc; j < new_argc; j++)
            free(new_argv[j]);
          free(new_argv);
          exit(1);
        }
        buf[i++] = c;
      }
    }
    if (new_argc + 1 >= curr_argc + 20) {
      fprintf(2, "xargs: too many arguments\n");
      for (int j = curr_argc; j < new_argc; j++)
        free(new_argv[j]);
      free(new_argv);
      exit(1);
    }
  }
  free(new_argv);
}

int
main(int argc, char* argv[])
{
  if(argc < 2)
  {
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }
  char* cmd = argv[1];
  xargs(cmd, &argv[1]);
  exit(0);
}

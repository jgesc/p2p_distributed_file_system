#include "threading.h"

int thread(void (* target)(void *), void * args)
{
  int pid;
  if(pid = fork())
  {
    return pid;
  }
  (*target)(args);
  exit(0);
  return 0;
}

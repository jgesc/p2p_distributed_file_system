#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threading.h"

void threaded_function(void * args)
{
  int i = *((int *)args);
  sleep(i);
}

int main(void)
{
  int i;
  for(i = 0; i < 10; i++)
  {
    thread(threaded_function, &i);
  }
}

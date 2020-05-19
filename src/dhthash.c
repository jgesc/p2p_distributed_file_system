#include "common/data_structures/hash.h"
#include <stdio.h>
#include <sys/mman.h>

int main(int argc, char ** argv)
{
  // Parse arguments
  if(argc != 2)
  {
    printf("Usage:\n\t%s FILE\n", argv[0]);
    return 0;
  }


  FILE * f = fopen(argv[1], "rb");
  if(!f)
  {
    perror("Can't open file");
    return 1;
  }
  fseek(f, 0L, SEEK_END);
  size_t fsz = ftell(f);
  rewind(f);

  uint8_t * d = malloc(fsz + 1);
  fread(d, 1, fsz, f);

  struct hash h = hash(d, fsz);
  printf("%016lx%016lx%016lx%016lx\t%016lx\n", h.a, h.b, h.c, h.d, hashreduce(&h));

  fclose(f);
}

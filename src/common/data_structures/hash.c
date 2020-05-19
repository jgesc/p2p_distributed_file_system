#include "hash.h"

struct hash hash(void * data, size_t l)
{
  uint32_t i;
  struct hash hstrct = {0};
  uint64_t * d = data;
  for(i = 0; i < l / 32; i++)
  {
    hstrct.a ^= d[i * 32 + 0];
    hstrct.b ^= d[i * 32 + 1];
    hstrct.c ^= d[i * 32 + 2];
    hstrct.d ^= d[i * 32 + 3];
  }

  // Last round
  struct hash tmp = {0};
  memcpy(&tmp, &(d[l / 32]), l % 32);
  hstrct.a ^= tmp.a;
  hstrct.b ^= tmp.b;
  hstrct.c ^= tmp.c;
  hstrct.d ^= tmp.d;

  return hstrct;
}

uint64_t hashreduce(struct hash * hash)
{
  return hash->a ^ hash->b ^ hash->c ^ hash->d;
}

#include "hash.h"

struct hash hash(void * data, size_t l)
{
  uint32_t i;
  struct hash hstrct = {0};
  uint64_t * d = data;
  for(i = 0; i <= l / 32; i++)
  {
    hstrct.a += d[i + 0] + hstrct.d;
    hstrct.b += d[i + 1] + hstrct.a;
    hstrct.c += d[i + 2] + hstrct.b;
    hstrct.d += d[i + 3] + hstrct.c;
  }

  // Last round
  struct hash tmp = {0};
  memcpy(&tmp, &(d[l / 32 + 1]), l % 32);
  hstrct.a += tmp.a ^ hstrct.d;
  hstrct.b += tmp.b ^ hstrct.a;
  hstrct.c += tmp.c ^ hstrct.b;
  hstrct.d += tmp.d ^ hstrct.c;

  return hstrct;
}

uint64_t hashreduce(struct hash * hash)
{
  return hash->a + hash->b + hash->c + hash->d;
}

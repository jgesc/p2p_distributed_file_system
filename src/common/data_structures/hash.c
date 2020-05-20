#include "hash.h"

struct hash hash(void * data, size_t l)
{
  uint32_t i;
  struct hash hstrct = {0};
  uint64_t * d = data;
  const uint64_t fnv_prime = 1099511628211ull;
  hstrct.a = 14695981039346656037ull;
  hstrct.b = 14695981739346656037ull;
  hstrct.c = 14695981039346756037ull;
  hstrct.d = 14795981039346656037ull;
  for(i = 0; i < l / 8; i++)
  {
    hstrct.a = (hstrct.a ^ d[i]) * fnv_prime;
    hstrct.b = (hstrct.b ^ d[i]) * fnv_prime;
    hstrct.c = (hstrct.c ^ d[i]) * fnv_prime;
    hstrct.d = (hstrct.d ^ d[i]) * fnv_prime;
  }

  return hstrct;
}

struct hash parsehash(char * strhash)
{
  struct hash hash = {0};
  if(strlen(strhash) != 64) return hash;
  sscanf(strhash, "%016lx%016lx%016lx%016lx", &hash.a, &hash.b, &hash.c, &hash.d);
  return hash;
}

uint64_t hashreduce(struct hash * hash)
{
  return hash->a;
}

int hashcmp(struct hash * a, struct hash * b)
{
  return (a->a == b->a) && (a->b == b->b) && (a->c == b->c) && (a->d == b->d);
}

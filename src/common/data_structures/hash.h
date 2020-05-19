#ifndef __HASH_H__
#define __HASH_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash
{
  uint64_t a;
  uint64_t b;
  uint64_t c;
  uint64_t d;
};

struct hash hash(void * data, size_t l);

struct hash parsehash(char * strhash);

uint64_t hashreduce(struct hash * hash);

int hashcmp(struct hash * a, struct hash * b);

#endif

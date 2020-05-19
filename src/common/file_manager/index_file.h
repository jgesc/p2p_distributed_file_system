#ifndef __INDEX_FILE_H__
#define __INDEX_FILE_H__

#include <stdint.h>

#include "../data_structures/hash.h"

struct idxfile
{
  char name[31];
  uint8_t fragcount;
  struct hash frags[255];
};

#endif

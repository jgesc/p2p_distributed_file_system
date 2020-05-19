#include "circular_static_list.h"

struct cstlist * cstl_new(uint32_t max)
{
  struct cstlist * l = calloc(1, sizeof(struct cstlist) + max * sizeof(uint64_t));
  l->max = max;
  return l;
}

void cstl_add(struct cstlist * l, uint64_t n)
{
  l->val[l->p] = n;
  l->p++;
  if(l->p >= l->max) l->p = 0;
}

int cstl_contains(struct cstlist * l, uint64_t n)
{
  uint32_t i;
  for(i = 0; i < l->max; i++)
    if(l->val[i] == n) return 1;
  return 0;
}

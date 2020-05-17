#include "static_list.h"

struct stlist * stl_new(uint32_t max, size_t elsize)
{
  struct stlist * list = malloc(max * elsize + sizeof(struct stlist));

  list->max = max;
  list->elsize = elsize;
  list->len = 0;

  return list;
}

int stl_add(struct stlist * l, void * d)
{
  if(l->len >= l->max) return 0;
  memcpy(l->data + l->len * l->elsize, d, l->elsize);
  l->len++;
  return 1;
}

int stl_contains(struct stlist * l, void * ele, int (*cmpfun)(void*, void*))
{
  int i;
  for(i = 0; i < l->len; i++)
    if((*cmpfun)(l->data + i * l->elsize, ele)) return 1;
  return 0;
}

void stl_remove(struct stlist * l, uint32_t idx)
{
  // Basic checks
  if(idx >= l->len || !l) return;
  // Overwrite
  if(l->len > 1 && idx < (l->len - 1))
    memcpy(l->data + idx * l->elsize, l->data + (l->len - 1) * l->elsize, l->elsize);
  l->len--;
}

void * stl_get(struct stlist * l, uint32_t n)
{
  if(n >= l->len) return NULL;
  else return l->data + n * l->elsize;
}

#include "peer_addr.h"

int cmppaddr(struct peer_addr * a, struct peer_addr * b)
{
  return a->id == b->id;
}

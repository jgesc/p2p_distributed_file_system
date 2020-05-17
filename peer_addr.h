#ifndef __PEER_ADDR_H__
#define __PEER_ADDR_H__

#include <netinet/in.h>

struct peer_addr
{
  unsigned long long int id;
  struct sockaddr_in addr;
};

#endif

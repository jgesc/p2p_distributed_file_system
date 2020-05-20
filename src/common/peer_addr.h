/** networking.h
 *  Address used in virtual network
 */

#ifndef __PEER_ADDR_H__
#define __PEER_ADDR_H__

#include <netinet/in.h>

struct peer_addr
{
  unsigned long long int id;
  struct sockaddr_in addr;
};

int cmppaddr(struct peer_addr * a, struct peer_addr * b);

#endif

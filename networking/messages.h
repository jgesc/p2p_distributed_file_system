#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "../peer_addr.h"
#include "../internal_state.h"
#include "packets.h"
#include "networking.h"

// Network joining message
struct msg_join
{
  struct peer_addr newpeer; // Who is the new node
  uint8_t reqcon; // How many peers wants to know
};

// Peer list message
struct msg_peers
{
  uint16_t count;
  struct peer_addr peers[];
};

void procmsg(struct internal_state * self, void * buffer);

#endif

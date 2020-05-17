#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "../peer_addr.h"
#include "../internal_state.h"
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

// Request to add to neighbors
struct msg_addme
{
  struct peer_addr peer;
};

// Handles packet at network-overlay level
int handlenetl(struct internal_state * self, void * buffer);

// Handles packet payload
void procmsg(struct internal_state * self, void * buffer);

#endif

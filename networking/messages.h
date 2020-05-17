#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "../peer_addr.h"
#include "../internal_state.h"
#include "packets.h"

#define MSG_JOIN 1

// Network joining message
struct msg_join
{
  struct peer_addr newpeer; // Who is the new node
  uint8_t reqcon; // How many peers wants to know
};

void procmsg(struct internal_state * self, void * buffer);

#endif

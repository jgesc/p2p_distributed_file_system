/** internal_state.h
 *  Node's persistent state
 */

#ifndef __INTERNAL_STATE_H__
#define __INTERNAL_STATE_H__

/// Includes

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "data_structures/static_list.h"
#include "data_structures/circular_static_list.h"
#include "peer_addr.h"

/// Structs

struct internal_state
{
  struct peer_addr selfaddr;
  struct stlist * neighbors;
  struct cstlist * bchist;
  char fpath[64];
  int sock;
};

/// Prototypes

// Allocates memory for the internal state
struct internal_state * init_self(short port);

// Frees an internal state memory block
void free_internal_state(struct internal_state * is);

// Meets a new peer, forgetting a random known peer if neccessary
void meet_new_peer(struct internal_state * self, struct peer_addr * peer);

// Prints known peer ids
void print_peers(struct internal_state * self);

#endif

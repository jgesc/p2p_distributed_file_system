#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "packets.h"

enum msgtype {JOIN, PEERS};

// Send singlecast
int send_sc(struct internal_state * self, enum msgtype cnttype, void * payload, uint16_t cntlen, struct peer_addr * dest);

// Send randomcast
int send_rc(struct internal_state * self, enum msgtype cnttype, void * payload, uint16_t cntlen, uint8_t hops);

// Send broadcast
int send_bc(struct internal_state * self, enum msgtype cnttype, void * payload, uint16_t cntlen, int16_t breadth);

#endif

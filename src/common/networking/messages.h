#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#define CONST_SHARDS 16

#include "../peer_addr.h"
#include "../internal_state.h"
#include "../file_manager/file_manager.h"
#include "../data_structures/hash.h"
#include "msgdefs.h"
#include "networking.h"

// Handles packet at network-overlay level
int handlenetl(struct internal_state * self, void * buffer);

// Handles packet payload
void procmsg(struct internal_state * self, void * buffer);

#endif

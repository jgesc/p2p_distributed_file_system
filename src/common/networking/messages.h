/** networking.h
 *  Handles behaviour on message reception
 */

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#define CONST_SHARDS 16

/// Includes

#include "../peer_addr.h"
#include "../internal_state.h"
#include "../file_manager/file_manager.h"
#include "../data_structures/hash.h"
#include "msgdefs.h"
#include "networking.h"

/// Prototypes

// Handles packet at network-overlay level
int handlenetl(struct internal_state * self, void * buffer);

// Handles packet payload
void procmsg(struct internal_state * self, void * buffer);

#endif

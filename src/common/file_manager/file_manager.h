/** file_manager.h
 *  Handles file read and write
 */

#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#define CONST_FILE_SIZE 8192

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../internal_state.h"
#include "../networking/msgdefs.h"
#include "../data_structures/hash.h"

int fm_exists(struct internal_state * self, void * msg_file);

int fm_store(struct internal_state * self, void * msg_file);

int fm_read(struct internal_state * self, void * msg_file);

#endif

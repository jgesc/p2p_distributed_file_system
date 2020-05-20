/** circular_static_list.h
 *  Data structure for a circular list in static memory
 */

#ifndef __CIRCULAR_STATIC_LIST_H__
#define __CIRCULAR_STATIC_LIST_H__

/// Includes

#include <stdint.h>
#include <stdlib.h>

/// Structs

struct cstlist
{
  uint32_t max;
  uint32_t p;
  uint64_t val[];
};

/// Prototypes

struct cstlist * cstl_new(uint32_t max);

void cstl_add(struct cstlist * l, uint64_t n);

int cstl_contains(struct cstlist * l, uint64_t n);

#endif

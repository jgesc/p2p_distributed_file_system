/** static_list.h
 *  List data structure in static memory
 */

#ifndef __STATIC_LIST_H__
#define __STATIC_LIST_H__

/// Includes

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/// Structures

// Static memory list struct
struct stlist
{
  uint32_t max; // Max slots
  uint32_t len; // Used slots
  size_t elsize; // Element size
  uint8_t data[];
};

/// Operations
// Creates a new list
struct stlist * stl_new(uint32_t max, size_t elsize);

// Adds an element to the list
int stl_add(struct stlist * l, void * d);

// Checks if an element is in the list
int stl_contains(struct stlist * l, void * ele, int (*cmpfun)(void*, void*));

// Removes an element from the list
void stl_remove(struct stlist * l, uint32_t idx);

// Gets an element from the list
void * stl_get(struct stlist * l, uint32_t n);

#endif

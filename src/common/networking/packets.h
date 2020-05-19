#ifndef __PACKETS_H__
#define __PACKETS_H__

/// Includes
#include <stdint.h>
#include <stdlib.h>

#include "../peer_addr.h"

/// Macro definitions
// Constants
#define HEADER_LEN sizeof(struct packet);

/// Enum definitions
// Header types
enum header_type {RC, BC, SC};

/// Struct definitions
// Randomcast header
struct randomcast_hdr
{
  uint8_t hops;
};

// Broadcast header
struct broadcast_hdr
{
  int16_t breadth;
  uint64_t uid;
};

// Singlecast header
struct singlecast_hdr
{

};

// Payload header
struct payload
{
  uint16_t cnttype; // Payload type
  uint16_t len; // Payload length
  uint8_t content[]; // Payload
};

// Generic packet struct
struct packet
{
  enum header_type hdrtype; // Header type
  union // Header content
  {
    struct randomcast_hdr hdr_rc;
    struct broadcast_hdr hdr_bc;
    struct singlecast_hdr hdr_sc;
  };
  struct peer_addr src; // Packet source
  struct payload payload;
};

/// Function prototypes
// Gets the packet total size in bytes
size_t pkt_len(struct packet * pckt);

#endif

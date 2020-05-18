#include "packets.h"

size_t pkt_len(struct packet * pckt)
{
  return HEADER_LEN + pckt->payload.len;
}

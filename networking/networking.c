#include "networking.h"

int send_sc(struct internal_state * self, enum msgtype cnttype, void * payload, uint16_t cntlen, struct peer_addr * dest)
{
  // Allocate memory in stack
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;

  pckt->hdrtype = SC;
  pckt->src = self->selfaddr;
  pckt->payload.cnttype = cnttype;
  pckt->payload.len = cntlen;
  memcpy(pckt->payload.content, payload, cntlen);
  return sendto(self->sock, pckt, sizeof(struct packet) + cntlen, 0, (const struct sockaddr *) (&(dest->addr)), sizeof(struct sockaddr_in));
}

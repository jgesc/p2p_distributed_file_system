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

int send_rc(struct internal_state * self, enum msgtype cnttype, void * payload, uint16_t cntlen, uint8_t hops)
{
  // Allocate memory in stack
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;

  // Fill packet fields
  pckt->hdrtype = RC;
  pckt->src = self->selfaddr;
  pckt->hdr_rc.hops = hops;
  pckt->payload.cnttype = cnttype;
  pckt->payload.len = cntlen;
  // Copy payload
  memcpy(pckt->payload.content, payload, cntlen);
  // Pick random target
  struct peer_addr * dest = stl_get(self->neighbors, rand() % self->neighbors->len);
  // Send
  return sendto(self->sock, pckt, sizeof(struct packet) + cntlen, 0, (const struct sockaddr *) (&(dest->addr)), sizeof(struct sockaddr_in));
}

int relay_rc(struct internal_state * self, struct packet * pckt)
{
  printf("Received randomcast with %d hops\n", pckt->hdr_rc.hops);
  // Decrease hop count
  pckt->hdr_rc.hops--;
  // Pick random target
  struct peer_addr * dest = stl_get(self->neighbors, rand() % self->neighbors->len);
  // Resend
  return sendto(self->sock, pckt, pkt_len(pckt), 0, (const struct sockaddr *) (&(dest->addr)), sizeof(struct sockaddr_in));
}

int send_bc(struct internal_state * self, enum msgtype cnttype, void * payload, uint16_t cntlen, int16_t breadth)
{
  // Allocate memory in stack
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;

  // Fill packet fields
  pckt->hdrtype = BC;
  pckt->src = self->selfaddr;
  pckt->hdr_bc.breadth = breadth;
  pckt->payload.cnttype = cnttype;
  pckt->payload.len = cntlen;
  pckt->hdr_bc.uid = ((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand();
  // Copy payload
  memcpy(pckt->payload.content, payload, cntlen);
  // Send
  struct peer_addr * dest;
  int i;
  for(i = 0; i < self->neighbors->len; i++)
  {
    dest = stl_get(self->neighbors, i);
    sendto(self->sock, pckt, sizeof(struct packet) + cntlen, 0, (const struct sockaddr *) (&(dest->addr)), sizeof(struct sockaddr_in));
  }
  cstl_add(self->bchist, pckt->hdr_bc.uid);

  return 0;
}

int relay_bc(struct internal_state * self, struct packet * pckt)
{
  printf("Received broadcast with %d breadth\n", pckt->hdr_bc.breadth);
  // Decrease hop count
  pckt->hdr_bc.breadth--;
  // Send
  struct peer_addr * dest;
  int i;
  for(i = 0; i < self->neighbors->len; i++)
  {
    dest = stl_get(self->neighbors, i);
    sendto(self->sock, pckt, pkt_len(pckt), 0, (const struct sockaddr *) (&(dest->addr)), sizeof(struct sockaddr_in));
  }
}

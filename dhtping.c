#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "common/internal_state.h"
#include "common/networking/networking.h"
#include "common/networking/messages.h"

int main(int argc, char ** argv)
{
  // Parse arguments
  if(argc != 5)
  {
    printf("Usage:\n\t%s PORT CAST_MODE HOST_IP HOST_PORT\n\n\tCast modes:\n\t1 - SC\n\t2 - RC\n\t3 - BC\n", argv[0]);
    return 0;
  }

  short port = atoi(argv[1]);
  int castmode = atoi(argv[2]);
  char * seedip = argv[3];
  short seedport = atoi(argv[4]);

  struct peer_addr seed;
  seed.addr.sin_family = AF_INET;
  seed.addr.sin_port = htons(seedport);
  seed.addr.sin_addr.s_addr = inet_addr(seedip);

  struct internal_state * self = init_self(port);

  struct msg_ping payload;
  srand(time(NULL));
  payload.echo = rand();

  uint8_t buffer[65536];
  struct sockaddr_in cliaddr;
  int addrlen;

  switch(castmode)
  {
    case 1:
    {
      printf("PING!\n");
      send_sc(self, PING, &payload, sizeof(struct msg_ping), &seed);
      recvfrom(self->sock, buffer, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
      struct msg_ping * pong = (void*)((struct packet *)buffer)->payload.content;
      if(pong->echo == payload.echo) printf("PONG!\n");
      break;
    }
    case 2:
    {
      printf("PING!\n");
      // Allocate memory in stack
      uint8_t _pckt[sizeof(struct packet) + sizeof(struct msg_ping)];
      struct packet * pckt = (struct packet *)_pckt;

      // Fill packet fields
      pckt->hdrtype = RC;
      pckt->src = self->selfaddr;
      pckt->hdr_rc.hops = 8;
      pckt->payload.cnttype = PING;
      pckt->payload.len = sizeof(struct msg_ping);
      // Fill payload
      ((struct msg_ping *)(pckt->payload.content))->echo = rand();
      // Send
      sendto(self->sock, pckt, sizeof(_pckt), 0, (const struct sockaddr *) (&(seed.addr)), sizeof(struct sockaddr_in));
      recvfrom(self->sock, buffer, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
      struct msg_ping * pong = (void*)((struct packet *)buffer)->payload.content;
      if(pong->echo == payload.echo) printf("PONG!\n");
      break;
    }
    case 3:
    {
      printf("PING!\n");
      // Allocate memory in stack
      uint8_t _pckt[65336];
      struct packet * pckt = (struct packet *)_pckt;

      // Fill packet fields
      pckt->hdrtype = BC;
      pckt->src = self->selfaddr;
      pckt->hdr_bc.breadth = 5;
      pckt->payload.cnttype = PING;
      pckt->payload.len = sizeof(struct msg_ping);
      pckt->hdr_bc.uid = ((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand();
      // Fill payload
      ((struct msg_ping *)(pckt->payload.content))->echo = rand();
      // Send
      sendto(self->sock, pckt, sizeof(_pckt), 0, (const struct sockaddr *) (&(seed.addr)), sizeof(struct sockaddr_in));
      recvfrom(self->sock, buffer, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
      struct msg_ping * pong = (void*)((struct packet *)buffer)->payload.content;
      uint32_t pongs = 0;
      if(pong->echo == payload.echo) printf("PONG %u!\n", pongs++);
      break;
    }

    default:
    {
      printf("Unknown castmode");
      break;
    }
  }
}

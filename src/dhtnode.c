#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "common/internal_state.h"
#include "common/networking/networking.h"
#include "common/networking/messages.h"

const int reqcon = 6;
const int rdcon = 6;
const int rdconhops = 4;

void nodeconnect(struct internal_state * self, struct peer_addr * seed)
{
  uint8_t buffer[65536];
  struct sockaddr_in cliaddr;
  int addrlen;

  printf("Connecting to seed\n");

  // Send JOIN message
  struct msg_join payload;
  payload.newpeer = self->selfaddr;
  payload.reqcon = reqcon;
  send_sc(self, JOIN, (void *)&payload, sizeof(struct msg_join), seed);

  // Wait for response
  recvfrom(self->sock, buffer, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
  struct packet * res = (void*)buffer;
  struct msg_peers * rescnt = (void*)res->payload.content;
  printf("Received %d new peers from seed\n", rescnt->count);
  procmsg(self, buffer);
  meet_new_peer(self, seed);

  // Send ADDME randomcasts
  int i;
  struct msg_addme payload2;
  payload2.peer = self->selfaddr;
  for(i = 0; i < rdcon; i++)
    send_rc(self, ADDME, (void *)&payload2, sizeof(struct msg_addme), rdconhops);
}

void node(struct peer_addr * seed, short port)
{
  // Initialize internal state
  struct internal_state * self = init_self(port);
  mkdir(self->fpath, 0700);

  // Connect
  if(seed) nodeconnect(self, seed);
  else printf("Started node as SERVER\n");

  // Listen to incoming packets
  uint8_t buffer[65536];
  struct sockaddr_in cliaddr;
  int addrlen;
  while(1)
  {
    recvfrom(self->sock, buffer, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(handlenetl(self, buffer))
      procmsg(self, buffer);
  }
}

int main(int argc, char ** argv)
{
  // Parse arguments
  if(argc != 4 && argc != 2)
  {
    printf("Usage:\n\t%s PORT [SEED_IP SEED_PORT]\n", argv[0]);
    return 0;
  }
  srand(time(NULL) + getpid());
  if(argc == 4)
  {
    short port = atoi(argv[1]);
    char * seedip = argv[2];
    short seedport = atoi(argv[3]);

    struct peer_addr seed;
    seed.addr.sin_family = AF_INET;
    seed.addr.sin_port = htons(seedport);
    seed.addr.sin_addr.s_addr = inet_addr(seedip);

    node(&seed, port);
  }
  else
  {
    short port = atoi(argv[1]);
    node(NULL, port);
  }

  return 0;
}

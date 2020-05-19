#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common/internal_state.h"
#include "common/file_manager/index_file.h"
#include "common/networking/msgdefs.h"
#include "common/networking/packets.h"

int main(int argc, char ** argv)
{
  // Parse args
  char * fname = argv[1];
  char * seedip = argv[2];
  short seedport = atoi(argv[3]);
  short port = atoi(argv[4]);
  struct peer_addr seed;
  seed.addr.sin_family = AF_INET;
  seed.addr.sin_port = htons(seedport);
  seed.addr.sin_addr.s_addr = inet_addr(seedip);

  srand(time(NULL));

  struct internal_state * self = init_self(port);

  FILE * f = fopen(fname, "rb");

  struct idxfile idx = {0};
  strcpy(idx.name, "Test file");

  uint8_t fragid = 0;
  struct msg_file msg = {0};
  int i;
  size_t rbytes;
  // Packet assembly buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;

  // Fill packet fields
  pckt->hdrtype = BC;
  pckt->src = self->selfaddr;
  pckt->hdr_bc.breadth = 25;
  pckt->payload.cnttype = FILEFRAG;
  pckt->payload.len = sizeof(struct msg_file);

  // Read all file
  while(rbytes = fread(msg.data, 1, CONST_FILE_SIZE, f))
  {
    // Generate uid
    pckt->hdr_bc.uid = getpid() * 10000 + (((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand());
    // Calculate hash and store
    msg.hash = hash(msg.data, CONST_FILE_SIZE);
    idx.frags[fragid] = msg.hash;
    // Copy payload
    memcpy(pckt->payload.content, &msg, sizeof(struct msg_file));

    printf("%016lx%016lx%016lx%016lx\t%lx\n", msg.hash.a, msg.hash.b, msg.hash.c, msg.hash.d, hashreduce(&msg.hash) % 16);
    // Send
    sendto(self->sock, pckt, pkt_len(pckt), 0, (const struct sockaddr *) (&(seed.addr)), sizeof(struct sockaddr_in));

    // Clear buffer
    memset(&msg, 0, sizeof(struct msg_file));
  }

  memcpy(msg.data, &idx, sizeof(struct idxfile));
  msg.hash = hash(msg.data, CONST_FILE_SIZE);
  // Copy payload
  memcpy(pckt->payload.content, &msg, sizeof(struct msg_file));
  printf("%016lx%016lx%016lx%016lx\t%lx\n", msg.hash.a, msg.hash.b, msg.hash.c, msg.hash.d, hashreduce(&msg.hash) % 16);
  // Generate uid
  pckt->hdr_bc.uid = getpid() * 10000 + (((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand());
  // Send
  sendto(self->sock, pckt, sizeof(_pckt), 0, (const struct sockaddr *) (&(seed.addr)), sizeof(struct sockaddr_in));

  fclose(f);
}

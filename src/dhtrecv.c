#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common/data_structures/hash.h"
#include "common/internal_state.h"
#include "common/networking/networking.h"
#include "common/file_manager/index_file.h"
#include "common/data_structures/static_list.h"

#define CONST_SHARDS 16

void populate_peer_list(struct internal_state * self, struct stlist ** peerlist)
{
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr = {0};
  int addrlen = 0;
  // Find
  struct msg_ping ping;
  ping.echo = rand();
  //printf("Searching %x ...\n", find.mod);
  send_bc(self, PING, &ping, sizeof(struct msg_ping), 3);
  int i;
  for(i = 0; i < 6; i++)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(!stl_contains(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src, (void*)cmppaddr));
      stl_add(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src);
  }
}

struct peer_addr find_mod(struct internal_state * self, uint16_t mod, struct stlist ** peerlist)
{
  // Check peer cache
  if(peerlist[mod]->len > 0)
    return *(struct peer_addr*)stl_get(peerlist[mod], rand() % peerlist[mod]->len);
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr = {0};
  int addrlen = 0;
  // Find
  struct msg_find find;
  find.src = self->selfaddr;
  find.mod = mod;
  //printf("Searching %x ...\n", find.mod);
  send_bc(self, FIND, &find, sizeof(struct msg_find), 3);
  while(1)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(!stl_contains(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src, (void*)cmppaddr));
      stl_add(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src);
    if(pckt->src.id % CONST_SHARDS == mod) return pckt->src;
  }
}

void download_fragment(struct internal_state * self, struct hash * hash, FILE * out, uint16_t bytes, struct stlist ** peerlist)
{
  // Find peer
  struct peer_addr peer = find_mod(self, hashreduce(hash) % CONST_SHARDS, peerlist);
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr = {0};
  int addrlen = 0;
  // Get File
  struct msg_get req;
  req.hash = *hash;
  send_sc(self, FILEGET, &req, sizeof(struct msg_get), &peer);
  while(1)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(!stl_contains(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src, (void*)cmppaddr));
      stl_add(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src);
    if(pckt->payload.cnttype == FILEFRAG && hashcmp(hash, &((struct msg_file *)pckt->payload.content)->hash)) break;
  }
  // Store
  fwrite(((struct msg_file *)pckt->payload.content)->data, 1, bytes, out);
  //printf("Downloaded %016lx%016lx%016lx%016lx\n", hash->a, hash->b, hash->c, hash->d);
}

int main(int argc, char ** argv)
{
  // Show help
  if(argc != 5)
  {
    printf("Usage:\n\t%s HASH SEED_IP SEED_PORT PORT\n", argv[0]);
    return 0;
  }

  // Parse args
  char * strhash = argv[1];
  char * seedip = argv[2];
  short seedport = atoi(argv[3]);
  short port = atoi(argv[4]);

  // Create seed address structure
  struct peer_addr seed;
  seed.addr.sin_family = AF_INET;
  seed.addr.sin_port = htons(seedport);
  seed.addr.sin_addr.s_addr = inet_addr(seedip);

  // Initialize random
  srand(time(NULL));
  // Initialize self
  struct internal_state * self = init_self(port);
  stl_add(self->neighbors, &seed);
  // Allocate index
  struct idxfile idx;
  // Peer list
  int i;
  struct stlist * peerlist[16];
  for(i = 0; i < 16; i++)
    peerlist[i] = stl_new(10, sizeof(struct peer_addr));
  populate_peer_list(self, peerlist);

  // Request
  struct msg_get req;
  req.hash = parsehash(strhash);

  // Find node
  struct peer_addr node = find_mod(self, hashreduce(&req.hash) % CONST_SHARDS, peerlist);

  // Request index
  send_sc(self, FILEGET, &req, sizeof(struct msg_get), &node);
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr = {0};
  int addrlen = 0;
  // Get index
  while(1)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(!stl_contains(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src, (void*)cmppaddr));
      stl_add(peerlist[pckt->src.id % CONST_SHARDS], &pckt->src);
    if(pckt->payload.cnttype == FILEFRAG && hashcmp(&req.hash, &((struct msg_file *)pckt->payload.content)->hash)) break;
  }
  memcpy(&idx, ((struct msg_file *)pckt->payload.content)->data, CONST_FILE_SIZE);
  printf("File '%s'\n\tFragments: %d\n\tSize: %d bytes\n", idx.name, idx.fragcount, CONST_FILE_SIZE * (idx.fragcount - 1) + idx.lfsize);

  // Download file fragments
  char fname[30] = {0};
  strncpy(fname, idx.name, 29);
  FILE * f = fopen(fname, "wb");
  for(i = 0; i < (idx.fragcount - 1); i++)
  {
    download_fragment(self, &idx.frags[i], f, CONST_FILE_SIZE, peerlist);
    putchar('#');
    fflush(stdout);
    //usleep(100000);
  }
  download_fragment(self, &idx.frags[i], f, idx.lfsize, peerlist);
  putchar('#');
  fclose(f);

  // Free peer list
  for(i = 0; i < 16; i++)
    free(peerlist[i]);

  printf("\nFile downloaded\n");
}

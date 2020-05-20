#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common/data_structures/hash.h"
#include "common/internal_state.h"
#include "common/networking/networking.h"
#include "common/file_manager/index_file.h"

#define CONST_SHARDS 16

struct peer_addr find_mod(struct internal_state * self, uint16_t mod)
{
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr;
  int addrlen;
  // Find
  struct msg_find find;
  find.src = self->selfaddr;
  find.mod = mod;
  //printf("Searching %x ...\n", find.mod);
  send_bc(self, FIND, &find, sizeof(struct msg_find), 3);
  while(1)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(pckt->src.id % CONST_SHARDS == mod) return pckt->src;
  }
}

void download_fragment(struct internal_state * self, struct hash * hash, FILE * out, uint16_t bytes)
{
  // Find peer
  struct peer_addr peer = find_mod(self, hashreduce(hash) % CONST_SHARDS);
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr;
  int addrlen;
  // Get File
  struct msg_get req;
  req.hash = *hash;
  send_sc(self, FILEGET, &req, sizeof(struct msg_get), &peer);
  while(1)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
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

  // Request
  struct msg_get req;
  req.hash = parsehash(strhash);

  // Find node
  struct peer_addr node = find_mod(self, hashreduce(&req.hash) % CONST_SHARDS);

  // Request index
  send_sc(self, FILEGET, &req, sizeof(struct msg_get), &node);
  // Packet buffer
  uint8_t _pckt[65336];
  struct packet * pckt = (struct packet *)_pckt;
  // Address buffer
  struct sockaddr_in cliaddr;
  int addrlen;
  // Get index
  while(1)
  {
    recvfrom(self->sock, _pckt, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
    if(pckt->payload.cnttype == FILEFRAG) break;
  }
  memcpy(&idx, ((struct msg_file *)pckt->payload.content)->data, CONST_FILE_SIZE);
  printf("File '%s'\n\tFragments: %d\n\tSize: %d bytes\n", idx.name, idx.fragcount, CONST_FILE_SIZE * (idx.fragcount - 1) + idx.lfsize);

  // Download file fragments
  char fname[30] = {0};
  strncpy(fname, idx.name, 29);
  FILE * f = fopen(fname, "wb");
  int i;
  for(i = 0; i < (idx.fragcount - 1); i++)
  {
    download_fragment(self, &idx.frags[i], f, CONST_FILE_SIZE);
    putchar('#');
    fflush(stdout);
    //usleep(100000);
  }
  download_fragment(self, &idx.frags[i], f, idx.lfsize);
  putchar('#');
  fclose(f);

  printf("\nFile downloaded\n");
}

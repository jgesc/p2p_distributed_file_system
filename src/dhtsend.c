#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common/internal_state.h"
#include "common/file_manager/index_file.h"
#include "common/networking/networking.h"
#include "common/networking/msgdefs.h"
#include "common/networking/packets.h"
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

int main(int argc, char ** argv)
{
  // Show help
  if(argc != 5)
  {
    printf("Usage:\n\t%s FILENAME SEED_IP SEED_PORT PORT\n", argv[0]);
    return 0;
  }

  // Parse args
  char * fname = argv[1];
  char * seedip = argv[2];
  short seedport = atoi(argv[3]);
  short port = atoi(argv[4]);

  // Create seed address structure
  struct peer_addr seed;
  seed.addr.sin_family = AF_INET;
  seed.addr.sin_port = htons(seedport);
  seed.addr.sin_addr.s_addr = inet_addr(seedip);
  // Open file
  FILE * f = fopen(fname, "rb");
  if(!f)
  {
    perror("Can't open file");
    return 1;
  }
  // Check file size
  fseek(f, 0L, SEEK_END);
  size_t fsz = ftell(f);
  if(fsz > 2088960)
  {
    printf("File exceeds size limit (%lu > %u)\n", fsz, 2088960);
    fclose(f);
    return 0;
  }
  rewind(f);
  // Initialize random
  srand(time(NULL));
  // Initialize self
  struct internal_state * self = init_self(port);
  stl_add(self->neighbors, &seed);
  // Initialize index structure
  struct idxfile idx = {0};
  strncpy(idx.name, fname, 29);
  uint8_t fragid = 0;
  // Initialize message buffer
  struct msg_file msg = {0};
  int i;
  size_t rbytes, lrbytes;

  struct peer_addr dest;
  struct stlist * peerlist[16];
  for(i = 0; i < 16; i++)
    peerlist[i] = stl_new(10, sizeof(struct peer_addr));
  populate_peer_list(self, peerlist);

  // Read all file
  while(rbytes = fread(msg.data, 1, CONST_FILE_SIZE, f))
  {
    // Record last read bytes
    lrbytes = rbytes;
    // Calculate hash and store
    msg.hash = hash(msg.data, CONST_FILE_SIZE);
    idx.frags[fragid] = msg.hash;
    fragid++;
    // Send
    dest = find_mod(self, hashreduce((void*)&msg.hash) % CONST_SHARDS, peerlist);
    send_sc(self, FILEFRAG, &msg, sizeof(struct msg_file), &dest);

    // Clear buffer
    memset(&msg, 0, sizeof(struct msg_file));
    usleep(100000);
    putchar('#');
    fflush(stdout);
  }
  //printf("\nFrags: %d\n", fragid);
  // Store fragment count
  idx.fragcount = fragid;
  idx.lfsize = (uint16_t)lrbytes;
  // Pack index in message
  memcpy(msg.data, &idx, sizeof(struct idxfile));
  msg.hash = hash(msg.data, CONST_FILE_SIZE);
  // Send
  dest = find_mod(self, hashreduce((void*)&msg.hash) % CONST_SHARDS, peerlist);
  send_sc(self, FILEFRAG, &msg, sizeof(struct msg_file), &dest);
  putchar('#');
  fflush(stdout);
  // Print result
  printf("\n%016lx%016lx%016lx%016lx\n", msg.hash.a, msg.hash.b, msg.hash.c, msg.hash.d);
  // Close file
  fclose(f);
  // Free peer list
  for(i = 0; i < 16; i++)
    free(peerlist[i]);
}

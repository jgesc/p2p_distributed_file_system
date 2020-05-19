#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common/internal_state.h"
#include "common/file_manager/index_file.h"
#include "common/networking/msgdefs.h"
#include "common/networking/packets.h"

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
  // Initialize random
  srand(time(NULL));
  // Initialize self
  struct internal_state * self = init_self(port);
  // Initialize index structure
  struct idxfile idx = {0};
  strncpy(idx.name, fname, 29);
  uint8_t fragid = 0;
  // Initialize message buffer
  struct msg_file msg = {0};
  int i;
  size_t rbytes, lrbytes;
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
    // Record last read bytes
    lrbytes = rbytes;
    // Generate uid
    pckt->hdr_bc.uid = getpid() * 10000 + (((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand());
    // Calculate hash and store
    msg.hash = hash(msg.data, CONST_FILE_SIZE);
    idx.frags[fragid] = msg.hash;
    fragid++;
    // Copy payload
    memcpy(pckt->payload.content, &msg, sizeof(struct msg_file));
    printf("SENT %016lx%016lx%016lx%016lx\t%lx\n", msg.hash.a, msg.hash.b, msg.hash.c, msg.hash.d, hashreduce(&msg.hash) % 16);
    // Send
    sendto(self->sock, pckt, pkt_len(pckt), 0, (const struct sockaddr *) (&(seed.addr)), sizeof(struct sockaddr_in));

    // Clear buffer
    memset(&msg, 0, sizeof(struct msg_file));
    usleep(100000);
  }
  // Store fragment count
  idx.fragcount = fragid;
  idx.lfsize = (uint16_t)lrbytes;
  // Pack index in message
  memcpy(msg.data, &idx, sizeof(struct idxfile));
  msg.hash = hash(msg.data, CONST_FILE_SIZE);
  // Copy payload
  memcpy(pckt->payload.content, &msg, sizeof(struct msg_file));
  // Generate uid
  pckt->hdr_bc.uid = getpid() * 10000 + (((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand());
  // Send
  sendto(self->sock, pckt, pkt_len(pckt), 0, (const struct sockaddr *) (&(seed.addr)), sizeof(struct sockaddr_in));
  // Print result
    printf("%016lx%016lx%016lx%016lx\n", msg.hash.a, msg.hash.b, msg.hash.c, msg.hash.d);
  // Close file
  fclose(f);
}

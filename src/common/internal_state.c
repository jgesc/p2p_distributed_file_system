#include "internal_state.h"

#define CONST_BCHIST_LEN 64

struct internal_state * init_self(short port)
{
  // Allocate internal state
  struct internal_state * state = malloc(sizeof(struct internal_state));
  state->neighbors = stl_new(32, sizeof(struct peer_addr));
  state->bchist = cstl_new(CONST_BCHIST_LEN);
  state->selfaddr.addr.sin_family = AF_INET;
  state->selfaddr.addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  state->selfaddr.addr.sin_port = htons(port);
  state->selfaddr.id = getpid() * 10000 + ((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | (rand() << 16) | rand();
  snprintf(state->fpath, 64, "nodo-%llu", state->selfaddr.id);
  mkdir(state->fpath, 0700);

  // Socket creation
  if((state->sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    perror("Can't create sockets");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
  if(bind(state->sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("Can't bind socket");
    exit(EXIT_FAILURE);
  }

  // Return
  return state;
}

void meet_new_peer(struct internal_state * self, struct peer_addr * peer)
{
  // Check if peer is self
  if(peer->id == self->selfaddr.id) return;
  // Check if already knows peer
  if(stl_contains(self->neighbors, peer, (void*)cmppaddr)) return;

  if(self->neighbors->len >= self->neighbors->max)
  {
    // Forget random peer
    uint32_t idx = rand() % self->neighbors->len;
    stl_remove(self->neighbors, idx);
  }

  stl_add(self->neighbors, peer);
}

void print_peers(struct internal_state * self)
{
  printf("==KNOWN PEERS==\n");
  int i;
  for(i = 0; i < self->neighbors->len; i++)
  {
    struct peer_addr * addr = stl_get(self->neighbors, i);
    printf("%lld\n", addr->id);
  }
  printf("===============\n");
}

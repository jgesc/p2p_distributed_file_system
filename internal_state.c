#include "internal_state.h"

struct internal_state * init_self(short port)
{
  // Allocate internal state
  struct internal_state * state = malloc(sizeof(struct internal_state));
  state->neighbors = stl_new(32, sizeof(struct peer_addr));

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

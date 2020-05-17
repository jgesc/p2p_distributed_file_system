#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "internal_state.h"
#include "networking/networking.h"

void nodeconnect(struct internal_state * self, struct peer_addr * seed)
{
  uint8_t buffer[65536];
  struct sockaddr_in cliaddr;
  int addrlen;

  printf("Connecting to seed\n");

  // Send JOIN message
  struct msg_join payload;
  payload.newpeer = self->selfaddr;
  payload.reqcon = 4;
  send_sc(self, JOIN, (void *)&payload, sizeof(struct msg_join), seed);

  // Wait for response
  recvfrom(self->sock, buffer, 65536, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addrlen);
  struct packet * res = (void*)buffer;
  struct msg_peers * rescnt = (void*)res->payload.content;
  printf("Received %d new peers from seed\n", rescnt->count);
}

void node(struct peer_addr * seed, short port)
{
  // Initialize internal state
  struct internal_state * self = init_self(port);

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
    procmsg(self, buffer);
  }
}

int main(int argc, char ** argv)
{
  struct peer_addr seed;
  seed.addr.sin_family = AF_INET;
  seed.addr.sin_port = htons(8888);
  seed.addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(argc == 1) node(NULL, 8888);
  else node(&seed, 8889);

  return 0;
}

void test_list()
{
  struct stlist * l = stl_new(10, 256);
  int i;
  char * str;

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  char a[256] = "Hello wordl!";
  stl_add(l, a);

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  char b[256] = "This is a test for static memory list";
  stl_add(l, b);

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  char c[256] = "If it works i will be very happy";
  stl_add(l, c);

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  char d[256] = "This is another test string";
  stl_add(l, d);

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  char e[256] = "This one is test string E";
  stl_add(l, e);

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  char f[256] = "And this was the last one!";
  stl_add(l, f);

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  printf("%d, ", stl_add(l, f));
  printf("%d, ", stl_add(l, f));
  printf("%d, ", stl_add(l, f));
  printf("%d, ", stl_add(l, f));
  printf("%d, ", stl_add(l, f));
  printf("%d, ", stl_add(l, f));
  printf("\n");

  for(i = 0; i < 10; i++)
    if((str = stl_get(l, i))) printf("%s\n", str);
  printf("\n");

  free(l);
}

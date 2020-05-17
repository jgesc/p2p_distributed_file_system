#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "internal_state.h"
#include "networking/networking.h"

void nodeconnect(struct internal_state * self, struct peer_addr * seed)
{
  printf("Connecting to seed");

  struct msg_join payload;
  send_sc(self, MSG_JOIN, (void *)&payload, sizeof(struct msg_join), seed);
}

void node(struct peer_addr * seed, short port)
{
  // Initialize internal state
  struct internal_state * self = init_self(port);

  // Connect
  if(seed) nodeconnect(self, seed);
  else printf("Started node as SERVER");

  // Listen to incoming packets
  uint8_t buffer[65536];
  while(1)
  {
    recvfrom(self->sock, buffer, 65536, MSG_WAITALL, NULL, 0);
    procmsg(self, buffer);
  }
}

int main(void)
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

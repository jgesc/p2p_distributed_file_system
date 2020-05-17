#include "messages.h"

void procmsg(struct internal_state * self, void * buffer)
{
  struct packet * msg = buffer;
  printf("Received %d\n", msg->payload.cnttype);
}

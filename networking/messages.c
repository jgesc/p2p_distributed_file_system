#include "messages.h"

void procmsg_join(struct internal_state * self, struct packet * msg)
{
  printf("RECEIVED Request to join\n");

  // Allocate response resources
  uint8_t buffer[65536];
  struct msg_peers * payload = (void*)buffer;

  // Parse request
  struct msg_join * msgpayload = (void*)msg->payload.content;

  if(msgpayload->reqcon >= self->neighbors->len)
  {
    payload->count = self->neighbors->len;
    memcpy(payload->peers, self->neighbors->data, self->neighbors->len * self->neighbors->elsize);
  }
  else
  {
    return;
    payload->count = msgpayload->reqcon;
  }
  send_sc(self, PEERS, payload, payload->count * self->neighbors->elsize, &msg->src);

  // Add peer to peer list
  meet_new_peer(self, &msg->src);
}

void procmsg(struct internal_state * self, void * buffer)
{
  struct packet * msg = buffer;

  printf("Received message\n");

  switch(msg->payload.cnttype)
  {
    case JOIN:
      procmsg_join(self, msg);
      break;
    default:
      printf("Unknown\n");
      break;
  }
}

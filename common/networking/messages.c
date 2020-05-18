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
    // TODO: implement random
    memcpy(payload->peers, self->neighbors->data, msgpayload->reqcon * self->neighbors->elsize);
    payload->count = msgpayload->reqcon;
  }
  send_sc(self, PEERS, payload, payload->count * self->neighbors->elsize, &msg->src);

  // Add peer to peer list
  meet_new_peer(self, &msg->src);
}

void procmsg_peers(struct internal_state * self, struct packet * msg)
{
  printf("RECEIVED Peer list\n");

  // Point to payload
  struct msg_peers * peerlist = (void*)msg->payload.content;

  // Iterate through peers
  int i;
  for(i = 0; i < peerlist->count; i++)
    meet_new_peer(self, &(peerlist->peers[i]));
}

void procmsg_addme(struct internal_state * self, struct packet * msg)
{
  printf("RECIVED addme\n");

  // Point to payload
  struct msg_addme * addme = (void*)msg->payload.content;

  //Add peer
  meet_new_peer(self, &(addme->peer));
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
    case PEERS:
      procmsg_peers(self, msg);
      break;
    case ADDME:
      procmsg_addme(self, msg);
      break;
    default:
      printf("Unknown\n");
      break;
  }
}

int handlenetl(struct internal_state * self, void * buffer)
{
  // Cast
  struct packet * msg = buffer;

  // Handle packet depending on net header type
  switch(msg->hdrtype)
  {
    // Singlecast
    case SC:
      return 1;
    // Randomcast
    case RC:
    {
      // Get header
      struct randomcast_hdr * hdr = &(msg->hdr_rc);
      // Check remaining hops
      if(hdr->hops > 0)
      {
        // Ignore message and relay
        relay_rc(self, buffer);
        return 0;
      }
      else
        // Process message
        printf("Received randomcast\n");
        return 1;
    }
    // Broadcast
      return 0; // TODO: implement
    default:
      return 0;
  }
}

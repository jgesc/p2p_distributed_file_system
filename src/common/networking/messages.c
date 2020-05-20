#include "messages.h"

void procmsg_join(struct internal_state * self, struct packet * msg)
{
  //printf("RECEIVED Request to join\n");

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
  //printf("RECEIVED Peer list\n");

  // Point to payload
  struct msg_peers * peerlist = (void*)msg->payload.content;

  // Iterate through peers
  int i;
  for(i = 0; i < peerlist->count; i++)
    meet_new_peer(self, &(peerlist->peers[i]));
}

void procmsg_addme(struct internal_state * self, struct packet * msg)
{
  //printf("RECIVED addme\n");

  // Point to payload
  struct msg_addme * addme = (void*)msg->payload.content;

  //Add peer
  meet_new_peer(self, &(addme->peer));
}

void procmsg_ping(struct internal_state * self, struct packet * msg)
{
  //printf("RECEIVED ping %lx\n", ((struct msg_ping *)(msg->payload.content))->echo);

  // Point to payload
  struct msg_ping * ping = (void*)msg->payload.content;

  // Pong
  send_sc(self, PING, ping, sizeof(struct msg_ping), &msg->src);
}

void procmsg_filefrag(struct internal_state * self, struct packet * msg)
{
  //printf("RECEIVED file fragment\n");

  // Point to payload
  struct msg_file * file = (void*)(msg->payload.content);

  // Check hash
  //printf("%016lx%016lx%016lx%016lx\t%lx\n", file->hash.a, file->hash.b, file->hash.c, file->hash.d, hashreduce(&file->hash) % 16);
  if(hashreduce((void*)&file->hash) % CONST_SHARDS != self->selfaddr.id % CONST_SHARDS)
  {
    //printf("%lx != %llx\n", hashreduce((void*)(&(file->hash))) % CONST_SHARDS, self->selfaddr.id % CONST_SHARDS);
    return; // Ignore
  }
  else
  {
    //printf("%lx == %llx\n", hashreduce((void*)(&(file->hash))) % CONST_SHARDS, self->selfaddr.id % CONST_SHARDS);
  }

  // If doesn't exist store
  if(!fm_exists(self, file))
  {
    //printf("STORE\n");
    fm_store(self, file);
  }
  else
  {
    printf("ALREADY STORED\n");
  }
}

void procmsg_fileget(struct internal_state * self, struct packet * msg)
{
  //printf("GET file fragment\n");

  // Point to payload
  struct msg_get * get = (void*)(msg->payload.content);
  // Check hash
  if(hashreduce((void*)&get->hash) % CONST_SHARDS != self->selfaddr.id % CONST_SHARDS)
    return; // Ignore
  // Allocate response
  struct msg_file file = {0};
  file.hash = get->hash;
  // Read file
  if(!fm_read(self, &file)) return;
  // Send file
  send_sc(self, FILEFRAG, &file, sizeof(struct msg_file), &msg->src);
}

void procmsg_find(struct internal_state * self, struct packet * msg)
{
  //printf("FIND received\n");

  // Point to payload
  struct msg_find * find = (void*)(msg->payload.content);
  // Check mod
  if(find->mod != self->selfaddr.id % CONST_SHARDS) return;
  // Answer
  send_sc(self, NONE, NULL, 0, &find->src);
}

void procmsg(struct internal_state * self, void * buffer)
{
  struct packet * msg = buffer;

  //printf("Received message\n");

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
    case PING:
      procmsg_ping(self, msg);
      break;
    case FILEFRAG:
      procmsg_filefrag(self, msg);
      break;
    case FILEGET:
      procmsg_fileget(self, msg);
      break;
    case FIND:
      procmsg_find(self, msg);
      break;
    default:
      //printf("Unknown\n");
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
      {
        // Process message
        //printf("Received randomcast\n");
        return 1;
      }
    }
    // Broadcast
    case BC:
    {
      // Get header
      struct broadcast_hdr * hdr = &(msg->hdr_bc);
      // Check if packet is repeated ignore
      if(cstl_contains(self->bchist, hdr->uid)) return 0;
      // Else add to history
      else cstl_add(self->bchist, hdr->uid);
      // Check if needs to relay
      if(hdr->breadth > 0)
        relay_bc(self, buffer);
      // Process message
      //printf("Received broadcast\n");
      return 1;
    }
    default:
      return 0;
  }
}

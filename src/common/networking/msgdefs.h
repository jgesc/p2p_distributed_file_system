#ifndef __MSGDEFS_H__
#define __MSGDEFS_H__

#include "../peer_addr.h"
#include "../data_structures/hash.h"

#define CONST_FILE_SIZE 8192

enum msgtype {JOIN, PEERS, ADDME, PING, FILEFRAG};

// Network joining message
struct msg_join
{
  struct peer_addr newpeer; // Who is the new node
  uint8_t reqcon; // How many peers wants to know
};

// Peer list message
struct msg_peers
{
  uint16_t count;
  struct peer_addr peers[];
};

// Request to add to neighbors
struct msg_addme
{
  struct peer_addr peer;
};

// Ping
struct msg_ping
{
  uint64_t echo;
};

// File
struct msg_file
{
  struct hash hash; //256 bit hash
  uint8_t data[CONST_FILE_SIZE];
};

#endif

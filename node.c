#include <stdio.h>
#include <stdlib.h>
#include <socket.h>

int debugPort = 12731;
char debugIp[] = "127.0.0.1";

int s;
unsigned short port;
struct sockaddr_in server;

struct debugmsg {
  char topic[8];
  char string[512];
};



int main(void) {

  LOG("ERROR", "values are : %d %d",4444,55555);
  return 0;
}

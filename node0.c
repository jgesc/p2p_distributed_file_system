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

#define LOG( topic,fmt,... )    \
        { \
            struct debugmsg msg = {0}; \
            msg.topic = topic; \
            snprintf(msg.string, 512, fmt, __VA_ARGS__);    \
        }

int main(void) {

  LOG("ERROR", "values are : %d %d",4444,55555);
  return 0;
}

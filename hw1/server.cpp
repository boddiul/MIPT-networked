#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "socket_tools.h"

int main(int argc, const char **argv)
{

  const char *port = "2022";

  int sfd = create_dgram_socket(nullptr, port, nullptr);

  if (sfd == -1)
    return 1;
  printf("listening!\n");



  sockaddr clientAddr;
  socklen_t clientAddrLen;
  char clientId;
  char* clientName;
  message msg;

  while (true)
  {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sfd, &readSet);

    timeval timeout = { 0, 100000 }; // 100 ms
    select(sfd + 1, &readSet, NULL, NULL, &timeout);

    if (FD_ISSET(sfd, &readSet))
    {


      struct sockaddr from;
      uint32_t addr_len = sizeof(from);
      
      bool rec = recieve_message(sfd,msg,&from,&addr_len);

      if (rec)
      {
        switch (msg.type)
        {
        case C_TO_S_INIT:
              clientAddr = from;
              clientAddrLen = addr_len;

              clientName = new char[msg.size];
              strcpy(clientName,msg.data);



              printf("User %s connected\n",clientName);
              clientId = 12;

              msg.type = S_TO_C_INIT;
              msg.size = 1;
              msg.data = new char[1] {clientId};

                
              send_message(sfd,&msg,&clientAddr,clientAddrLen);
          
          break;
        
          case C_TO_S_TEXT:
              printf("Message from %s: %s\n",clientName,msg.data);
          break;
        }

        
      }
    }
  }
  return 0;
}

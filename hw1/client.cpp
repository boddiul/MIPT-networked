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

  addrinfo serverAddrInfo;
  char myId = -1;


  int sfd = create_dgram_socket("localhost", port, &serverAddrInfo);

  if (sfd == -1)
  {
    printf("Cannot create a socket\n");
    return 1;
  }
  else
  {
      std::string input;
      message msg = {};

      printf("Enter your name:");
      std::getline(std::cin, input);

      msg.type = C_TO_S_INIT;
      msg.size = (uint)input.size();
      msg.data = new char[input.size()];
      strcpy(msg.data, input.c_str());
      bool ok = send_message(sfd,&msg,serverAddrInfo.ai_addr,serverAddrInfo.ai_addrlen);
      
        

      while (true)
      {

          
          if (myId!=-1)
          {
              printf("Enter message:");
              std::getline(std::cin, input);

              msg.type = C_TO_S_TEXT;
              msg.size = (uint)input.size();
              msg.data = new char[input.size()];
              strcpy(msg.data, input.c_str());
              bool ok = send_message(sfd,&msg,serverAddrInfo.ai_addr,serverAddrInfo.ai_addrlen);
          }
          
          fd_set readSet;
          FD_ZERO(&readSet);
          FD_SET(sfd, &readSet);

          timeval timeout = { 0, 100000 }; // 100 ms
          select(sfd + 1, &readSet, NULL, NULL, &timeout);

          if (FD_ISSET(sfd, &readSet))
          {


            bool rec = recieve_message(sfd,msg,serverAddrInfo.ai_addr,&serverAddrInfo.ai_addrlen);
            if (rec)
            {
              switch (msg.type)
              {
              case S_TO_C_INIT:
                  myId = msg.data[0];
                  printf("CONNECTION COMPLETE: My ID is %d\n",myId);
                break;
              
              default:
                break;
              }
            }
              
          }
          

        


      }

  }
  
  return 0;
}

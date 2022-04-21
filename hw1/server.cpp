#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "socket_tools.h"

#include <vector>

struct user {
  char id;
  char* name;
  sockaddr sockAddr;
  socklen_t sockLen;
};

int main(int argc, const char **argv)
{

  const char *port = "2022";

  int sfd = create_dgram_socket(nullptr, port, nullptr);

  if (sfd == -1)
    return 1;
  printf("listening!\n");



  std::vector<user> users;

  


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
        user newUser;
        switch (msg.type)
        {
        case C_TO_S_INIT:
              newUser = {.id = (char)users.size()};
              newUser.name = new char[msg.size];
              strcpy(newUser.name,msg.data);

              newUser.sockAddr = from;
              newUser.sockLen = addr_len;

              users.push_back(newUser);

              printf("User %s connected\n",newUser.name);

              msg.type = S_TO_C_INIT;
              msg.size = 1;
              msg.data = new char[1] {newUser.id};
                
              send_message(sfd,&msg,&newUser.sockAddr,newUser.sockLen);
          
          break;
        
          case C_TO_S_TEXT:
              for (int i=0;i<users.size();i++)
                  if (msg.data[0]==users[i].id)
                  {
                      printf("Message from %s: %s\n",users[i].name,&msg.data[1]);
                  }
          break;
        }

        
      }
    }
  }
  return 0;
}

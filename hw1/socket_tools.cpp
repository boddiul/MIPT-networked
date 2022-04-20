#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>

#include "socket_tools.h"


bool send_message(int sfd,message * msg,const sockaddr * sockAddr,socklen_t sockAddrLen)
{

    uint sz = msg->size+2;
    char *arr = new char[sz];
    arr[0] = msg->type;
    arr[1] = msg->size;

    for (int i=0;i<msg->size;i++)
        arr[i+2] = msg->data[i];

    
    //printf("%u %u\n",arr[0],arr[1]);
    
    ssize_t res = sendto(sfd, arr, sz, 0, sockAddr, sockAddrLen);


    return res != -1;
}

bool recieve_message(int sfd, message& msg,sockaddr * sockAddr,socklen_t * sockAddrLen)
{

    constexpr size_t buf_size = 1000;
    static char buffer[buf_size];
    memset(buffer, 0, buf_size);

    ssize_t numBytes = recvfrom(sfd, buffer, buf_size - 1, 0, sockAddr, sockAddrLen);



    if (numBytes == 0)
      return false;
    else
      {
        msg.type = (MessageType)buffer[0];
        msg.size = buffer[1];
        msg.data = new char[msg.size];

        for (int i=0;i<msg.size;i++)
          msg.data[i] = buffer[2+i];


        return true;
      }

}

// Adaptation of linux man page: https://linux.die.net/man/3/getaddrinfo
static int get_dgram_socket(addrinfo *addr, bool should_bind, addrinfo *res_addr)
{
  for (addrinfo *ptr = addr; ptr != nullptr; ptr = ptr->ai_next)
  {
    if (ptr->ai_family != AF_INET || ptr->ai_socktype != SOCK_DGRAM || ptr->ai_protocol != IPPROTO_UDP)
      continue;
    int sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (sfd == -1)
      continue;

    fcntl(sfd, F_SETFL, O_NONBLOCK);

    int trueVal = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &trueVal, sizeof(int));

    if (res_addr)
      *res_addr = *ptr;
    if (!should_bind)
      return sfd;

    if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
      return sfd;

    close(sfd);
  }
  return -1;
}

int create_dgram_socket(const char *address, const char *port, addrinfo *res_addr)
{
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));

  bool isListener = !address;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  if (isListener)
    hints.ai_flags = AI_PASSIVE;

  addrinfo *result = nullptr;
  if (getaddrinfo(address, port, &hints, &result) != 0)
    return 1;

  int sfd = get_dgram_socket(result, isListener, res_addr);

  //freeaddrinfo(result);
  return sfd;
}


#pragma once

struct addrinfo;

enum MessageType { C_TO_S_INIT, S_TO_C_INIT, C_TO_S_TEXT, S_TO_C_CHECK_ALIVE, C_TO_S_ALIVE };

struct message
{
  MessageType type;
  
  uint size;
  char* data;
};

bool send_message(int sfd,message * msg,const sockaddr * sockAddr,socklen_t sockAddrLen);
bool recieve_message(int sfd, message& msg,sockaddr * sockAddr,socklen_t * sockAddrLen);

int create_dgram_socket(const char *address, const char *port, addrinfo *res_addr);

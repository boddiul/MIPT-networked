#pragma once
#include <vector>

struct addrinfo;

enum MessageType { CLIENT_TO_LOBBY_COMMAND, LOBBY_TO_CLIENT_HOST_LINK, SERVER_TO_CLIENT_INIT };

struct message
{
  MessageType type;
  std::vector<std::string> data;

};

void send_message(message * msg, ENetPeer * peer,uint channel,bool isReliable);
void data_to_message(enet_uint8 * data, message &msg);


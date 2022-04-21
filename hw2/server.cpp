#include <enet/enet.h>
#include <iostream>
#include <string.h>

#include "tools.h"

struct user {
    uint id;
    std::string name;
    ENetPeer * peer;
};

const int gameServerPort = 12349;

int main(int argc, const char **argv)
{
  if (enet_initialize() != 0)
  {
    printf("Cannot init ENet");
    return 1;
  }
  ENetAddress address;


  address.host = ENET_HOST_ANY;
  address.port = gameServerPort;

  ENetHost *server = enet_host_create(&address, 32, 2, 0, 0);

  if (!server)
  {
    printf("Cannot create ENet server\n");
    return 1;
  }
  else
  {
    
    printf("Game server started\n");
  }

  std::vector<user> users;
  user newUser;
  message msg;

  while (true)
  {
    ENetEvent event;
    while (enet_host_service(server, &event, 10) > 0)
    {
      switch (event.type)
      {
      case ENET_EVENT_TYPE_CONNECT:

        newUser = {.id = 100+(uint)users.size(),
                  .name = "Player"+std::to_string(users.size()),
                  .peer = event.peer};

        users.push_back(newUser);

        printf("Connection with %x:%u established. ID: %u; Name: %s\n", 
                event.peer->address.host,
                event.peer->address.port,
                newUser.id,
                newUser.name.c_str());
        
        msg.type = SERVER_TO_CLIENT_INIT;
        msg.data = {std::to_string(newUser.id),newUser.name};

        send_message(&msg,newUser.peer,0,true);

        break;
      case ENET_EVENT_TYPE_RECEIVE:
        printf("Recieved something: '%s'\n", event.packet->data);

        enet_packet_destroy(event.packet);
        break;
      default:
        break;
      };
    }
  }

  enet_host_destroy(server);

  atexit(enet_deinitialize);
  return 0;
}


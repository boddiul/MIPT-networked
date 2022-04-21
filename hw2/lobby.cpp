#include <enet/enet.h>
#include <iostream>
#include <string.h>

#include "tools.h"

const char* startCommand = "/start";
const char* gameServerHost = "localhost";
const uint gameServerPort = 12349;
const uint lobbyPort = 10887;


int main(int argc, const char **argv)
{
  if (enet_initialize() != 0)
  {
    printf("Cannot init ENet");
    return 1;
  }
  ENetAddress address;


  address.host = ENET_HOST_ANY;
  address.port = lobbyPort;

  ENetHost *server = enet_host_create(&address, 32, 2, 0, 0);

  if (!server)
  {
    printf("Cannot create ENet server\n");
    return 1;
  }
  else
  {
    printf("Lobby started\n");
  }


  bool gameStarted = false;

  message msg;

  while (true)
  {
    ENetEvent event;
    while (enet_host_service(server, &event, 10) > 0)
    {
      switch (event.type)
      {
      case ENET_EVENT_TYPE_CONNECT:
        printf("Connection with %x:%u established\n", event.peer->address.host, event.peer->address.port);

        if (gameStarted)
        {
            msg.type = LOBBY_TO_CLIENT_HOST_LINK;
            msg.data = {gameServerHost, std::to_string(gameServerPort)};
            send_message(&msg,event.peer,0,true);
        }


        break;
      case ENET_EVENT_TYPE_RECEIVE:

        data_to_message(event.packet->data,msg);

        if (msg.type == CLIENT_TO_LOBBY_COMMAND)
        {
          
          printf("Recieved COMMAND: '%s'\n", msg.data[0].c_str());

          if (msg.data[0] == startCommand)
          {
              for (int i=0;i< server->connectedPeers; i++)
                {
                  msg.type = LOBBY_TO_CLIENT_HOST_LINK;
                  msg.data = {gameServerHost, std::to_string(gameServerPort)};
                  send_message(&msg,&server->peers[i],0,true);
                }
              gameStarted = true;
          }
        }

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


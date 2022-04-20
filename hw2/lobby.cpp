#include <enet/enet.h>
#include <iostream>
#include <string.h>


const char* startCommand = "/start";
const char* gameServerAddress = "localhost:12349";
const int lobbyPort = 10887;


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
            ENetPacket *packet = enet_packet_create(gameServerAddress, strlen(gameServerAddress), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, packet); 
        }


        break;
      case ENET_EVENT_TYPE_RECEIVE:
        if (strcmp((char *)event.packet->data,startCommand)==0)
        {

          printf("Recieved START: '%s'\n", event.packet->data);
          for (int i=0;i< server->connectedPeers; i++)
            {
              ENetPacket *packet = enet_packet_create(gameServerAddress, strlen(gameServerAddress), ENET_PACKET_FLAG_RELIABLE);
              enet_peer_send(&server->peers[i], 0, packet); 
            }
          gameStarted = true;
        }
        else
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

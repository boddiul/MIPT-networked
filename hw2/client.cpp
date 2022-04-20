#include <enet/enet.h>
#include <iostream>
#include <string.h>

const char* startCommand = "/start";
const char* lobbyHost = "localhost";
const int lobbyPort = 10887;

int main(int argc, const char **argv)
{
  if (enet_initialize() != 0)
  {
    printf("Cannot init ENet");
    return 1;
  }

  ENetHost *client = enet_host_create(nullptr, 2, 2, 0, 0);
  if (!client)
  {
    printf("Cannot create ENet client\n");
    return 1;
  }

  ENetAddress address;
  enet_address_set_host(&address, lobbyHost);
  address.port = lobbyPort;

  ENetPeer *serverPeer = enet_host_connect(client, &address, 2, 0);
  if (!serverPeer)
  {
    printf("Cannot connect to lobby\n");
    return 1;
  }
  else
  {
    printf("Connected to lobby\n");
  }

  uint32_t timeStart = enet_time_get();

  bool inLobby = true;

  bool willCallStart = false;
  uint32_t timeToCallStart = 0;

  if (argc>1)
  {
    
      willCallStart = true;

      int timeSec = strtol(argv[1],nullptr,10);
      timeToCallStart = timeSec*1000;

      printf("Will call start after %u seconds\n",timeSec);
  }


  uint32_t lastUpdateSendTime = timeStart;



  bool connected = false;
  while (true)
  {
    ENetEvent event;
    while (enet_host_service(client, &event, 10) > 0)
    {
      switch (event.type)
      {
      case ENET_EVENT_TYPE_CONNECT:
        printf("Connection with %x:%u established\n", event.peer->address.host, event.peer->address.port);
        connected = true;
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        if (inLobby)
        {
            

            char *line = (char *)event.packet->data;
            char *p;
            char *sHost = strtok_r(line, ":", &p);
            char *sPort = strtok_r(NULL, ":", &p);

            enet_address_set_host(&address, sHost);
            address.port = strtol(sPort,nullptr,10);


            serverPeer = enet_host_connect(client, &address, 2, 0);
            if (!serverPeer)
            {
              printf("Cannot connect to server\n");
              return 1;
            }
            else
            {
              printf("Connected to server\n");
            }
        }
        enet_packet_destroy(event.packet);  

        break;
      default:
        break;
      };
    }
    if (connected)
    {
      uint32_t curTime = enet_time_get();

      if (willCallStart && curTime - timeStart > timeToCallStart)
      {
        
          printf("%s\n",startCommand);

          ENetPacket *packet = enet_packet_create(startCommand, strlen(startCommand), ENET_PACKET_FLAG_RELIABLE);
          enet_peer_send(serverPeer, 0, packet);
          willCallStart = false;
      }

      if (curTime - lastUpdateSendTime > 1000)
      {
        lastUpdateSendTime = curTime;
      }
      


    }
  }
  return 0;
}

#include <enet/enet.h>
#include <iostream>
#include <string.h>

#include "tools.h"

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
  bool inGame = false;

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

  message msg = {};


  std::string myName;
  uint myId;

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
      
        data_to_message(event.packet->data,msg);

        if (inLobby)
        {
            if (msg.type == LOBBY_TO_CLIENT_HOST_LINK)
            {
      
                enet_address_set_host(&address, msg.data[0].c_str());
                address.port = std::atoi(msg.data[1].c_str());

                serverPeer = enet_host_connect(client, &address, 2, 0);
                if (!serverPeer)
                {
                  printf("Cannot connect to server\n");
                  return 1;
                }
                else
                {
                  printf("Connected to server\n");
                  inLobby = false;
                  inGame = true;
                }
            }

        }
        else if (inGame)
        {
            if (msg.type == SERVER_TO_CLIENT_INIT)
            {
                myId = std::atoi(msg.data[0].c_str());
                myName = msg.data[1];
                printf("My ID: %u; My name: %s\n",myId,myName.c_str());
                printf("Other players (total %lu):\n",(msg.data.size()-2)/2);
                for (int i=0;i<(msg.data.size()-2)/2;i++)
                    printf("(%s) [%s]\n",msg.data[3+2*i].c_str(),msg.data[2+2*i].c_str());

            }
            else if (msg.type == SERVER_TO_CLIENT_NEW_CONNECTED)
            {
                printf("New Player connected (%s) [%s]\n",msg.data[1].c_str(),msg.data[0].c_str());
            }
            else if (msg.type == SERVER_TO_CLIENT_UPDATE)
            {
                printf("Server time (%s)\n",msg.data[0].c_str());
            }
            else if (msg.type == SERVER_TO_CLIENT_PING_LIST)
            {
                printf("PING LIST (id:ping): ");
                for (int i=0;i<(msg.data.size())/2;i++)
                    printf("(%s:%s)",msg.data[2*i].c_str(),msg.data[2*i+1].c_str());
                printf("\n");
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

          msg.type = CLIENT_TO_LOBBY_COMMAND;
          msg.data = {startCommand};

          send_message(&msg,serverPeer,0,true);
          
          willCallStart = false;
      }

      if (curTime - lastUpdateSendTime > 1000)
      {
        lastUpdateSendTime = curTime;
        msg = {.type = CLIENT_TO_SERVER_UPDATE,.data = {std::to_string(curTime-timeStart)}};
        
        send_message(&msg,serverPeer,0,true);
      }
      


    }
  }
  return 0;
}

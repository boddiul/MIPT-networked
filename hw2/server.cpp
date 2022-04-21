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

  uint32_t timeStart = enet_time_get();
  uint32_t lastUpdateSendTime = timeStart;

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

        for (int i=0;i<users.size()-1;i++)
        {
            msg.data.push_back(std::to_string(users[i].id));
            msg.data.push_back(users[i].name);
        }

        send_message(&msg,newUser.peer,0,true);

        msg.type = SERVER_TO_CLIENT_NEW_CONNECTED;
        msg.data = {std::to_string(newUser.id),newUser.name};
        
        for (int i=0;i< server->connectedPeers; i++)
            if (&server->peers[i] != event.peer)
                send_message(&msg,&server->peers[i],0,true);


        break;
      case ENET_EVENT_TYPE_RECEIVE:
        data_to_message(event.packet->data,msg);


        if (msg.type == CLIENT_TO_SERVER_UPDATE)
        {
            for (int k=0;k<users.size();k++)
              if (users[k].peer==event.peer)
              {
                printf("Player (%s) time: %s\n",users[k].name.c_str(),msg.data[0].c_str());
                break;
              }

        }

        enet_packet_destroy(event.packet);
        break;
      default:
        break;
      };
    }
    if (server->connectedPeers>0)
    {
      uint32_t curTime = enet_time_get();

      if (curTime - lastUpdateSendTime > 1000)
      {
        lastUpdateSendTime = curTime;
        msg = {.type = SERVER_TO_CLIENT_UPDATE,.data = {std::to_string(curTime-timeStart)}};

        for (int i=0;i< server->connectedPeers; i++)
            send_message(&msg,&server->peers[i],0,false);

        msg.type = SERVER_TO_CLIENT_PING_LIST;
        msg.data = {};

        for (int i=0;i<users.size();i++)
        {
            msg.data.push_back(std::to_string(users[i].id));
            msg.data.push_back(std::to_string(users[i].peer->roundTripTime));
        }

        for (int i=0;i< server->connectedPeers; i++)
            send_message(&msg,&server->peers[i],0,false);


      }
      


    }
  }

  enet_host_destroy(server);

  atexit(enet_deinitialize);
  return 0;
}


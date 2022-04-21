#include <enet/enet.h>
#include <vector>
#include <string>

#include "tools.h"


void send_message(message * msg, ENetPeer * peer,uint channel,bool isReliable)
{   

    uint sz = 2;

    for (int i=0;i<msg->data.size();i++)
        sz+=msg->data[i].length()+1;
    
    enet_uint8 *arr = new enet_uint8[sz];
    arr[0] = msg->type;
    arr[1] = msg->data.size();
    uint k = 2;

    for (int i=0;i<msg->data.size();i++)
    {
        arr[k] = msg->data[i].length();
        k+=1;

        for (int j=0;j<msg->data[i].length();j++)
        {
            arr[k] = msg->data[i][j];
            k+=1;
        }

    }

    ENetPacket *packet = enet_packet_create(arr, sz, isReliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
    enet_peer_send(peer, channel, packet);
}

void data_to_message(enet_uint8 * data, message &msg)
{

    msg.type = (MessageType)data[0];
    msg.data = {};

    uint vectorSize = data[1];
    uint k = 2;
    while (vectorSize>0)
    {
        uint strLen = data[k];
        msg.data.push_back(std::string(&data[k+1], &data[k+strLen+1]));
        k+=strLen+1;
        vectorSize-=1;
    }

}
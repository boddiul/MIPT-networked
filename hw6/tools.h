
#pragma once
#include <vector>

#include <termios.h>
#include <unistd.h>

int khbit();
void nonblock(int state);
bool keyState(int key);
std::string getCommand(int pauseKey);


struct addrinfo;

enum MessageType { 
    CLIENT_TO_MATCHMAKER_GET_ROOM_LIST,
    CLIENT_TO_MATCHMAKER_SELECT_ROOM,
    CLIENT_TO_MATCHMAKER_LEAVE_ROOM,
    
    MATCHMAKER_TO_CLIENT_ROOM_LIST,
    MATCHMAKER_TO_CLIENT_HOST_LINK,
    MATCHMAKER_TO_CLIENT_MESSAGE,

    SERVER_TO_MATCHMAKER_READY,
    MATCHMAKER_TO_SERVER_START,

    SERVER_TO_CLIENT_INIT,
    SERVER_TO_CLIENT_NEW_CONNECTED,

    CLIENT_TO_SERVER_UPDATE,

    SERVER_TO_CLIENT_UPDATE,
    SERVER_TO_CLIENT_PING_LIST
     };

struct message
{
  MessageType type;
  std::vector<std::string> data;

};

void send_message(message * msg, ENetPeer * peer,uint channel,bool isReliable);
void data_to_message(enet_uint8 * data, message &msg);


#ifndef _SOCKETMANAGER_
#define _SOCKETMANAGER_

#include <Windows.h>
#include "../message/Message.h"
/* Perform networks operations: send and receive messages*/


#define PSIZE_FIELD_SIZE 4


typedef struct _SocketManager
{
    SOCKET s;
    PCSTR server;
    u_short port;
} SOCKET_MANAGER, *PSOCKET_MANAGER;


void initSocketManager(PSOCKET_MANAGER pSockManager);
BOOL sendMessage(PSOCKET_MANAGER sManager, PMESSAGE message);
void receiveMessage(PSOCKET_MANAGER sManager, PMESSAGE message);
BOOL destroySocketManager(PSOCKET_MANAGER pSockManager);

#endif
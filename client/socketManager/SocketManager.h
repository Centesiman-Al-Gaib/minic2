#ifndef _SOCKETMANAGER_
#define _SOCKETMANAGER_

#include <Windows.h>
#include "../message/Message.h"
#include "../message/MessageQueue.h"
/* Perform networks operations: send and receive messages*/


#define PSIZE_FIELD_SIZE 4


typedef struct _SocketManager
{
    SOCKET s;
    PMESSAGE_QUEUE queueMessageReceived;
    PMESSAGE_QUEUE queueMessagesToSend;
} SOCKET_MANAGER, *PSOCKET_MANAGER;


PSOCKET_MANAGER initSocketManager(PMESSAGE_QUEUE queueMessageReceived, PMESSAGE_QUEUE queueMessagesToSend);
BOOL sendMessage(PSOCKET_MANAGER sManager, PMESSAGE message);
void receiveMessage(PSOCKET_MANAGER sManager);
BOOL destroySocketManager(PSOCKET_MANAGER pSockManager);

#endif
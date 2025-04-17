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

typedef struct _SendFunctionArgs
{
    PSOCKET_MANAGER sManager;
} SEND_FUNCTION_ARGS, *PSEND_FUNCTION_ARGS;

typedef struct _RecvFunctionArgs
{
    PSOCKET_MANAGER sManager;
} RECV_FUNCTION_ARGS, *PRECV_FUNCTION_ARGS;

PSOCKET_MANAGER initSocketManager(PMESSAGE_QUEUE queueMessageReceived, PMESSAGE_QUEUE queueMessagesToSend);
BOOL destroySocketManager(PSOCKET_MANAGER pSockManager);
BOOL sendMessageThread(PSEND_FUNCTION_ARGS args);
void receiveMessageThread(PRECV_FUNCTION_ARGS args);



#endif
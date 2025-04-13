#ifndef _SOCKETMANAGER_
#define _SOCKETMANAGER_

#include <Windows.h>

/* Perform networks operations: send and receive messages*/


#define SERVER "127.0.0.1"
#define PORT 8888
#define PSIZE_FIELD_SIZE 4


typedef struct _SocketManager
{
    SOCKET s;
    PCSTR server;
    u_short port;
} SOCKET_MANAGER, *PSOCKET_MANAGER;

typedef struct _Message
{
    BYTE type;
    DWORD size;
    PBYTE payload;
} MESSAGE, *PMESSAGE;



void createSocketManager(PSOCKET_MANAGER pSockManager);
BOOL sendMessage(PSOCKET_MANAGER sManager, PMESSAGE message);
void receiveMessage(PSOCKET_MANAGER sManager, PMESSAGE message);

#endif
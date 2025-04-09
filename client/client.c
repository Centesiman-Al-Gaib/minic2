// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <stdio.h>

typedef struct _Message
{
    BYTE type;
    DWORD *pSize;
    PBYTE payload;
} Message, *PMESSAGE;


BOOL sendMessage(SOCKET s, PMESSAGE message)
{

    PBYTE data = NULL;  
    DWORD *pSize = message->pSize;
    DWORD sizeToAlloc = *pSize + sizeof(DWORD) + 1;
    data = (PBYTE)VirtualAlloc(NULL, sizeToAlloc, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

    if (data == NULL)
    {
        printf("VirtualAlloc failed 0x%X", GetLastError());
        return FALSE;
    }
    
    data[0] = message->type;
    memcpy((data + 1), message->pSize, sizeof(DWORD));
    memcpy((data + 1 + sizeof(DWORD)), message->payload, *pSize);

    send(s, data, sizeToAlloc, 0);
    return TRUE;
}


int main()
{

    WSADATA wsaData = {0};
    int sStartUp = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (sStartUp != 0)
    {
        printf("Socket initialization failed -> %i\n", sStartUp);
        return 1;
    }

    SOCKET sock = INVALID_SOCKET;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed -> 0x%i\n", WSAGetLastError());
        return 1;
    }

    printf("[+] Socket creation successful...\n");
    printf("[+] Trying to connect to server...\n");
    LPCSTR data = "127.0.0.1";
    u_short port = 8888;
    struct sockaddr_in sockAddr =
    {
        .sin_family = AF_INET, 
        .sin_port = htons(port)
    };

    if (inet_pton(AF_INET, data, &sockAddr.sin_addr) != 1)
    {
        printf("inet_pton failed -> %i\n", WSAGetLastError());
        return 1;
    };

    if (connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) != 0)
    {
        printf("connect failed -> %i\n", WSAGetLastError());
        return 1;
    };

    unsigned char *testing = "This is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the firstThis is the first";
    DWORD size = (DWORD)strlen(testing);
    Message debug = 
    {
        .type = 0xA,
        .pSize = &size,
        .payload = testing
    };
    sendMessage(sock, &debug);
    closesocket(sock);
    return 0;
}
#include "Message.h"
#include <Windows.h>
#include <stdio.h>

PMESSAGE createMessage(BYTE type, DWORD size, PBYTE payload)
{
    PMESSAGE message = (PMESSAGE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if(message == NULL)
    {   
        printf("[-] HeapAlloc Failed %i", GetLastError());
        return NULL;
    }

    message->type = type;
    message->size = size;
    message->payload = payload;
    return message;
};

BOOL freeMessage(PMESSAGE message)
{
    if(!HeapFree(GetProcessHeap(), 0, message->payload) && !HeapFree(GetProcessHeap(), 0, message))
    {   
        printf("[-] HeapFree Failed %i", GetLastError());
        return FALSE;
    }

    return TRUE;
};
#include <Windows.h>
#include <stdio.h>
#include "Message.h"
#include "../memoryCollector/MemoryCollector.h"
PMESSAGE createMessage(BYTE type, DWORD size, PBYTE payload)
{
    PMESSAGE message = (PMESSAGE)allocMemory(sizeof(MESSAGE));
    if(message == NULL)
    {   
        return NULL;
    }

    message->type = type;
    message->size = size;
    message->payload = payload;
    return message;
};

BOOL freeMessage(PMESSAGE message)
{
    freeMemory(message->payload); 
    freeMemory(message);
    return TRUE;
};
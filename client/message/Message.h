#ifndef _MESSAGETYPE_
#define _MESSAGETYPE_
#include <Windows.h>

#define PING_MESSAGE_TYPE 0 
#define REQ_TASK_TYPE


typedef struct _Message
{
    BYTE type;
    DWORD size;
    PBYTE payload;
} MESSAGE, *PMESSAGE;

PMESSAGE createMessage(BYTE type, DWORD size, PBYTE payload);
BOOL freeMessage(PMESSAGE message);

#endif
#ifndef _MESSAGETYPE_
#define _MESSAGETYPE_

#define INIT_TYPE 0x01
#define REQ_TASK_TYPE
#include <Windows.h>

typedef struct _Message
{
    BYTE type;
    DWORD size;
    PBYTE payload;
} MESSAGE, *PMESSAGE;

PMESSAGE createMessage(BYTE type, DWORD size, PBYTE payload);
BOOL freeMessage(PMESSAGE message);

#endif
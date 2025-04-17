#include <Windows.h>
#include <stdio.h>
#include "Task.h"
#include "../memoryCollector/MemoryCollector.h"
#include "../message/Message.h"

PMESSAGE pingServer()
{   
    printf("[*] pingServer\n");
    PBYTE payload = (PBYTE)allocMemory(sizeof("PING"));
    DWORD paylaodSize = sizeof("PING");
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payload);
}

PMESSAGE initAgentId(PBYTE payload)
{
    printf("initAgentId");
};

PMESSAGE injectViaDllTask(PBYTE payload)
{
    printf("injectViaDllTask");
};

PMESSAGE injectViaApcTask(PBYTE payload)
{
    printf("injectViaApcTask");
};

PMESSAGE injectViaFileMappingTask(PBYTE payload)
{
    printf("injectViaFileMappingTask");
};

PMESSAGE injectViaClassicProcessTask(PBYTE payload)
{
    printf("injectViaClassicProcessTask");
};
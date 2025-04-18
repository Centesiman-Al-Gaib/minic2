#include <Windows.h>
#include <stdio.h>
#include "Task.h"
#include "../memoryCollector/MemoryCollector.h"
#include "../message/Message.h"

PMESSAGE pingServer()
{   
    PBYTE payload = (PBYTE)allocMemory(sizeof("PING"));
    DWORD paylaodSize = sizeof("PING");
    memcpy(payload, "PING\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payload);
}

PMESSAGE initAgentId(PBYTE payload)
{
    printf("initAgentId");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("initAgentId"));
    DWORD paylaodSize = sizeof("initAgentId");
    memcpy(payloadS, "initAgentId\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaDllTask(PBYTE payload)
{
    printf("injectViaDllTask");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaDllTask"));
    DWORD paylaodSize = sizeof("injectViaDllTask");
    memcpy(payloadS, "injectViaDllTask\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaApcTask(PBYTE payload)
{
    printf("injectViaApcTask");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaApcTask"));
    DWORD paylaodSize = sizeof("injectViaApcTask");
    memcpy(payloadS, "injectViaApcTask\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaFileMappingTask(PBYTE payload)
{
    printf("injectViaFileMappingTask");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaFileMappingTask"));
    DWORD paylaodSize = sizeof("injectViaFileMappingTask");
    memcpy(payloadS, "injectViaFileMappingTask\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaClassicProcessTask(PBYTE payload)
{
    printf("injectViaClassicProcessTask");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask"));
    DWORD paylaodSize = sizeof("injectViaClassicProcessTask");
    memcpy(payloadS, "injectViaClassicProcessTask\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};
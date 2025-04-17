#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "socketManager/SocketManager.h"
#include "taskManager/TaskManager.h"
#include "crypto/Crypto.h"
#include "message/Message.h"
#include "message/MessageQueue.h"

int main(int argc, char** argv)
{      
    PMESSAGE_QUEUE queueToReceive = initQueue();
    PMESSAGE_QUEUE queueToSend = initQueue();
    PSOCKET_MANAGER pSockManager = initSocketManager(queueToReceive, queueToSend);
    PTASK_MANAGER pTaskManager = initTaskManager(queueToReceive, queueToSend);



    destroyTaskManager(pTaskManager);
    destroySocketManager(pSockManager);
    destroyQueue(queueToReceive);
    destroyQueue(queueToSend);
    return 0;   
}
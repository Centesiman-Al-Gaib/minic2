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

    SEND_FUNCTION_ARGS sManagerThreadArgs =
    {
        .sManager = pSockManager   
    };
    DWORD sendThreadId = 0;
    HANDLE sendThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendMessageThread, &sManagerThreadArgs, 0, &sendThreadId);
    RECV_FUNCTION_ARGS srcvManagerThreadArgs =
    {
        .sManager = pSockManager   
    };
    DWORD recvThreadId = 0;
    HANDLE recvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)receiveMessageThread, &srcvManagerThreadArgs, 0, &recvThreadId);
    
    PROCESS_MESSAGE_ARGS tMangerThreadArgs =
    {
        .pTaskManager = pTaskManager   
    };
    DWORD tManagerThread = 0;
    HANDLE taskThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)processMessageThread, &tMangerThreadArgs, 0, &tManagerThread);

    HANDLE threads[] = {sendThread,recvThread,taskThread};
    WaitForMultipleObjects(3, threads, TRUE, INFINITE);
    destroyTaskManager(pTaskManager);
    destroySocketManager(pSockManager);
    destroyQueue(queueToReceive);
    destroyQueue(queueToSend);
    return 0;   
}
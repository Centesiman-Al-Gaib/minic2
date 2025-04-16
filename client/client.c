#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "socketManager/SocketManager.h"
#include "crypto/Crypto.h"
#include "message/Message.h"
#include "message/MessageQueue.h"

int main(int argc, char** argv)
{      
    PMESSAGE_QUEUE queueToReceive = initQueue();
    PMESSAGE_QUEUE queueToSend = initQueue();
    PSOCKET_MANAGER sManager = initSocketManager(queueToReceive, queueToSend);

    destroySocketManager(sManager);
    destroyQueue(queueToReceive);
    destroyQueue(queueToSend);
    return 0;   
}
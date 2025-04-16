#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "socketManager/SocketManager.h"
#include "crypto/Crypto.h"
#include "message/Message.h"
#include "message/MessageQueue.h"

int main(int argc, char** argv)
{      
    MESSAGE_QUEUE queue = {0};
    SOCKET_MANAGER sManager = {0};

    initQueue(&queue);
    initSocketManager(&sManager);

    destroySocketManager(&sManager);
    destroyQueue(&queue);
    return 0;   
}
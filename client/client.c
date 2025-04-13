#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "SocketManager.h"
#include "crypto/Crypto.h"

int main(int argc, char** argv)
{      
    SOCKET_MANAGER sManager = {0};
    createSocketManager(&sManager);
    return 0;   
}
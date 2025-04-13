#include <Windows.h>
#include <stdio.h>
#include "Definitions.h"
#include "crypto/Crypto.h"

PCSTR definitions[] = {"HASH_CONNECT_FUNCTION","HASH_SEND_FUNCTION","HASH_RECV_FUNCTION"};
PCSTR definitionsValues[] = {"connect","send","recv"};

int main()
{
    for(int i = 0; i < sizeof(definitions)/sizeof(PCSTR); i++)
    {
        printf("#define %s\t%lu\n",definitions[i],hashAscii(definitionsValues[i]));
    }

}
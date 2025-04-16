#include <Windows.h>
#include <stdio.h>
#include "MemoryCollector.h"

LPVOID allocMemory(DWORD sizeToAlloc)
{
    if(sizeToAlloc <= 0)
    {
        return NULL;
    }

    LPVOID memoryToReturn = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeToAlloc);
    if(memoryToReturn == NULL)
    {
        printf("[-] HeapAlloc failed %i", GetLastError());
        return FALSE;
    }

    return memoryToReturn;
};

void freeMemory(LPVOID memoryToFree)
{
    if(memoryToFree == NULL || !HeapFree(GetProcessHeap(), 0, memoryToFree))
    {
        printf("[-] HeapFree failed %i", GetLastError());
        exit(1);
    }

};


BOOL freeAll()
{
    return TRUE;
};
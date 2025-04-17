#ifndef _MEMORY_
#define _MEMORY_
#include <Windows.h>

typedef struct _MemoryQueueNode{
    LPVOID memoryPointer;
    struct _MemoryQueueNode *next;
}MEMORY_QUEUE_NODE, *PMEMORY_QUEUE_NODE;

typedef struct _MemoryQueue{
    PMEMORY_QUEUE_NODE last;
    PMEMORY_QUEUE_NODE first;
}MEMORY_QUEUE, *PMEMORY_QUEUE;


LPVOID allocMemory(DWORD sizeToAlloc);
void freeMemory(LPVOID memoryToFree);
BOOL freeAll();

#endif
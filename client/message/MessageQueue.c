#include "MessageQueue.h"
#include "Message.h"
#include <stdio.h>
#include <Windows.h>

void initQueue(PMESSAGE_QUEUE queue)
{   
    PMESSAGE_QUEUE queueOut = (PMESSAGE_QUEUE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE_QUEUE));
    queue = queueOut; 
    if(queueOut == NULL)
    {
        printf("[-] HeapAlloc failed %i", GetLastError());
        return;
    }
    queueOut->first = NULL;
    queueOut->last = NULL;
};

BOOL push(PMESSAGE_QUEUE queue, PMESSAGE messageToPush)
{

    PMESSAGE_QUEUE_NODE newElement = (PMESSAGE_QUEUE_NODE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE_QUEUE_NODE));
    if(newElement == NULL)
    {
        printf("[-] HeapAlloc failed %i", GetLastError());
        return FALSE;
    }
    newElement->message = messageToPush; // Set the message in the node
    newElement->next = NULL; // Set as the last node in the queue

    /* EMPTY QUEUE */
    if(queue->last == NULL && queue->first == NULL)
    {
        queue->first = newElement;
        queue->last = newElement;
    }
    else
    {
        queue->last->next = newElement;
        queue->last = newElement;
    }

    return TRUE;


};

PMESSAGE pop(PMESSAGE_QUEUE queue)
{
    if(queue->first == NULL)
    {
        return NULL;
    }

    PMESSAGE_QUEUE_NODE firstNode = queue->first; // Obtain pointer to the first so we don't loose it when updating the new first node
    PMESSAGE messageToReturn = queue->first->message; //Obtain pointer so we don't loose it uppon freeing the node
    queue->first =  firstNode->next; // Update the first node. Now the first is the next to the former first node

    if(queue->first == NULL)
    {
        queue->last = NULL;
    }

    if(!HeapFree(GetProcessHeap(), 0, firstNode))
    {
        printf("[-] HeapFree failed %i", GetLastError());
        return FALSE;
    }

    return messageToReturn;


};

BOOL destroyQueue(PMESSAGE_QUEUE queue)
{
    return TRUE;
};
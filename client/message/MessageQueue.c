#include <stdio.h>
#include <Windows.h>
#include "MessageQueue.h"
#include "Message.h"
#include "../memoryCollector/MemoryCollector.h"



PMESSAGE_QUEUE initQueue()
{   
    PMESSAGE_QUEUE queue = (PMESSAGE_QUEUE)allocMemory(sizeof(MESSAGE_QUEUE));
    if(queue == NULL)
    {
        return NULL;
    }
    queue->first = NULL;
    queue->last = NULL;

    return queue;
};

BOOL destroyQueue(PMESSAGE_QUEUE queue)
{
    freeMemory(queue);
    return TRUE;
};

BOOL push(PMESSAGE_QUEUE queue, PMESSAGE messageToPush)
{
    PMESSAGE_QUEUE_NODE newElement = (PMESSAGE_QUEUE_NODE)allocMemory(sizeof(MESSAGE_QUEUE_NODE));
    if(newElement == NULL)
    {
        return FALSE;
    }
    newElement->message = messageToPush; // Set the message in the node
    newElement->next = NULL; // Set as the last node in the queue

    if(queue->last == NULL && queue->first == NULL) // Queue is empty
    {
        queue->first = newElement;
        queue->last = newElement;
    }
    else  // Any other case queue has, at least, one element 
    {
        queue->last->next = newElement;
        queue->last = newElement;
    }

    return TRUE;


};

PMESSAGE pop(PMESSAGE_QUEUE queue)
{
    if(queue->first == NULL) // If queue is empty nothing is returned
    {
        return NULL;
    }

    PMESSAGE_QUEUE_NODE firstNode = queue->first; // Obtain pointer to the first so we don't loose it when updating the new first node
    PMESSAGE messageToReturn = queue->first->message; // Obtain pointer so we don't loose it uppon freeing the node
    queue->first =  firstNode->next; // Update the first node. Now the first is the next to the former first node

    if(queue->first == NULL)
    {
        queue->last = NULL;
    }

    freeMemory(firstNode);
    return messageToReturn;
};

#ifndef _MESSAGEQUEUE_
#define _MESSAGEQUEUE_
#include "Message.h"
#include <Windows.h>

typedef struct _MessageQueueNode{
    PMESSAGE message;
    struct _MessageQueueNode *next;
}MESSAGE_QUEUE_NODE, *PMESSAGE_QUEUE_NODE;

typedef struct _MessageQueue{
    PMESSAGE_QUEUE_NODE last;
    PMESSAGE_QUEUE_NODE first;
}MESSAGE_QUEUE, *PMESSAGE_QUEUE;


PMESSAGE_QUEUE initQueue();
BOOL push(PMESSAGE_QUEUE queue, PMESSAGE messageToPush);
PMESSAGE pop(PMESSAGE_QUEUE queue);
BOOL destroyQueue(PMESSAGE_QUEUE queue);

#endif
#ifndef __TASKQUEUE__
#define __TASKQUEUE__
#include "ds.h"
#include "path.h"
typedef struct node_s {
    int temfd;
    timeWheel_t* ptimeWheel;
    int* findKickIdx; 
    int netfd;
    cmd_t cmd;
    MYSQL *mysql;
    pathStack_t* ppathStack;
    struct node_s* pNext;
} node_t;
typedef struct taskQueue_s {
    node_t * pFront;
    node_t * pRear;
    int queueSize;
} taskQueue_t;
int taskQueueInit(taskQueue_t * pqueue);
int enQueue(int netfd,taskQueue_t *pqueue,pathStack_t* ppathStack,MYSQL* mysql,cmd_t* cmd,timeWheel_t* ptimeWheel, int* findKickIdx,int temfd);
int deQueue(taskQueue_t *pqueue);
#endif

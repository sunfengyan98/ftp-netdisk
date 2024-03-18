
#include "taskQueue.h"
#include "ds.h"
#include "path.h"
int taskQueueInit(taskQueue_t * pqueue){
    PRINT_FUNCTION_LOG();
    bzero(pqueue,sizeof(taskQueue_t));
    return 0;
}
int enQueue(int netfd,taskQueue_t *pqueue,pathStack_t* ppathStack,
MYSQL* mysql,cmd_t* pcmd,timeWheel_t* ptimeWheel, int* findKickIdx,int temfd){
    PRINT_FUNCTION_LOG();
    node_t * pNew = (node_t *)calloc(1,sizeof(node_t));
    pNew->ptimeWheel=ptimeWheel;
    pNew->findKickIdx=findKickIdx;
    pNew->netfd=netfd;
    pNew->ppathStack = ppathStack;
    pNew->temfd=temfd;
    memcpy(&pNew->cmd,pcmd,sizeof(cmd_t));
    pNew->mysql=mysql;
    if(pqueue->queueSize == 0){
        pqueue->pFront = pNew;
        pqueue->pRear = pNew;
    }
    else{
        pqueue->pRear->pNext = pNew;
        pqueue->pRear = pNew;
    }
    ++pqueue->queueSize;
    return 0;
}
int deQueue(taskQueue_t *pqueue){
    PRINT_FUNCTION_LOG();
    node_t * pCur = pqueue->pFront;
    pqueue->pFront = pCur->pNext;
    if(pqueue->queueSize == 1){
       pqueue->pRear = NULL;
   }
   PRINT_LINE_LOG(free);
   free(pCur);
   --pqueue->queueSize;
   return 0;
}

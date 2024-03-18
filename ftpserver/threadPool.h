#ifndef __THREADPOOL__
#define __THREADPOOL__

#include <func.h>
#include "worker.h"
#include "taskQueue.h"
typedef struct threadPool_s {
    // 记录所有的子线程信息
    tidArr_t tidArr;
    // 任务队列
    taskQueue_t taskQueue;
    // 锁:
    pthread_mutex_t mutex;
    // 条件变量
    pthread_cond_t cond;
    // 退出标志位
    int exitFlag;
} threadPool_t;
int parsingShortCommand(int netfd, cmd_t* pcmd,pathStack_t* ppathStack,MYSQL * mysql,threadPool_t*  pthreadPool,int idx,char name[][32]);
int parsingLongCommand(int netfd, cmd_t* pcmd,pathStack_t* ppathStack,MYSQL * mysql,
threadPool_t*  pthreadPool,int idx,char name[][32],int temfd,timeWheel_t* ptimeWheel, int* findKickIdx);
int threadPoolInit(threadPool_t *pthreadPool, int workerNum);
int makeWorker(threadPool_t *pthreadPool);
int tcpInit(const char *ip,const char* port, int *psockfd);
int epollAdd(int epfd, int fd);
int epollDel(int epfd, int fd);

#endif

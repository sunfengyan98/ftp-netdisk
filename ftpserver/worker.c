#include "worker.h"
#include "taskQueue.h"
#include "threadPool.h"
#include "path.h"
#include "password.h"
#include "func.h"
#include "puts_gets.h"
#include "timewheel.h"

int tidArrInit(tidArr_t * ptidArr, int workerNum){
    PRINT_FUNCTION_LOG();
    // 申请内存 存储每个子线程的tid
    ptidArr->arr = (pthread_t *)calloc(workerNum,sizeof(pthread_t));
    ptidArr->workerNum = workerNum;
    return 0;
}
void unlock(void *arg){
    PRINT_FUNCTION_LOG();
    threadPool_t * pthreadPool = (threadPool_t *)arg;
    printf("unlock!\n");
    PRINT_LINE_LOG(pthread_mutex_unlock);
    pthread_mutex_unlock(&pthreadPool->mutex);
}
void * threadFunc(void *arg){
    PRINT_FUNCTION_LOG();

    threadPool_t * pthreadPool = (threadPool_t *)arg;
    pathStack_t pathStack;
    while(1){
        PRINT_LINE_LOG(pthread_mutex_lock);
        pthread_mutex_lock(&pthreadPool->mutex);
        int netfd;
        cmd_t cmd;
        bzero(&cmd,sizeof(cmd_t));
        //pthread_cleanup_push(unlock,pthreadPool);
        while(pthreadPool->exitFlag == 0 && pthreadPool->taskQueue.queueSize <= 0){
            pthread_cond_wait(&pthreadPool->cond, &pthreadPool->mutex);
        }
        if(pthreadPool->exitFlag == 1){
            printf("I am child, I am going to exit!\n");
            pthread_mutex_unlock(&pthreadPool->mutex);
            pthread_exit(NULL);
        }
        //Todo
        //此处定义一个结构体接受出队资源
        int temfd=pthreadPool->taskQueue.pFront->temfd;
        timeWheel_t* ptimeWheel=pthreadPool->taskQueue.pFront->ptimeWheel;
        int* findKickIdx=pthreadPool->taskQueue.pFront->findKickIdx;
        netfd = pthreadPool->taskQueue.pFront->netfd;

        memcpy(&cmd,&pthreadPool->taskQueue.pFront->cmd,sizeof(cmd_t));
        MYSQL* mysql=pthreadPool->taskQueue.pFront->mysql;
        pathStack_t* ppathStack=pthreadPool->taskQueue.pFront->ppathStack;
        printf("I am worker, I got a netfd = %d\n", netfd);
        PRINT_LINE_LOG();
        deQueue(&pthreadPool->taskQueue);
        pthread_mutex_unlock(&pthreadPool->mutex);
        //pthread_cleanup_pop(1);
        // 执行业务
        if(cmd.cmdType==CMD_PUTS){
            PRINT_LINE_LOG();
            int ret=server_puts(netfd,ppathStack,&cmd,mysql);
            if(ret==-1){
                close(netfd);
            }
        }
        else if(cmd.cmdType==CMD_GETS){
            printf("begin get server!\n");
            PRINT_LINE_LOG();
            int ret=server_gets(netfd,ppathStack,&cmd,mysql);
            if(ret==-1){
                close(netfd);
            }
        }
        PRINT_LINE_LOG();
        timewheelupdate_useractive(ptimeWheel, findKickIdx,temfd);
        close(netfd);
    }
}

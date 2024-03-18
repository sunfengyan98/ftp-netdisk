#include "worker.h"
#include "threadPool.h"
int threadPoolInit(threadPool_t *pthreadPool, int workerNum){
    PRINT_FUNCTION_LOG();

    PRINT_LINE_LOG();
    tidArrInit(&pthreadPool->tidArr,workerNum);
    PRINT_LINE_LOG();
    taskQueueInit(&pthreadPool->taskQueue);
    pthread_mutex_init(&pthreadPool->mutex,NULL);
    pthread_cond_init(&pthreadPool->cond,NULL);
    pthreadPool->exitFlag = 0;
    
    return 0;
}
int makeWorker(threadPool_t *pthreadPool){
    PRINT_FUNCTION_LOG();
    for(int i = 0; i < pthreadPool->tidArr.workerNum; ++i){
        PRINT_LINE_LOG();
        pthread_create(&pthreadPool->tidArr.arr[i], NULL,threadFunc, pthreadPool);
    }
    return 0;
}

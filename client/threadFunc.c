#include "threadFunc.h"
#include "tcpClientInit.h"
#include "puts_gets.h"
#include "getLocalAddr.h"
#include "transMessage.h"
void *threadFunc(void *arg)
{

    threadInfo_t *pthreadInfo = (threadInfo_t *)arg;
    int filesockfd;

    tcpClientInit(&filesockfd, pthreadInfo->arg1, pthreadInfo->arg2);

    if (pthreadInfo->longcmd.cmdType == CMD_PUTS)
    {
        
        train_t train;
        bzero(&train, sizeof(train_t));
        transLongCommand(filesockfd, pthreadInfo);
        recv(filesockfd, &train, sizeof(train_t), MSG_WAITALL);
        printf("begin puts files.\n");

        int ret = putsfiles(&pthreadInfo->longcmd, filesockfd);
        if (ret != 0)
        {
            printf("putsfiles failed!\n");
        }
    }
    else if (pthreadInfo->longcmd.cmdType == CMD_GETS)
    {
        transLongCommand(filesockfd, pthreadInfo);
        printf("begin gets files.\n");
        int ret = getsfiles(&pthreadInfo->longcmd, filesockfd);
        if (ret != 0)
        {
            printf("getsfiles failed!\n");
        }
    }

    close(filesockfd);

    return NULL;
}

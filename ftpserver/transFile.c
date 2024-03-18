#include "threadPool.h"
#include "worker.h"
#include "path.h"
#include "puts_gets.h"
#include "quitCurrentUser.h"
#include "removeFile.h"
#include "makeDir.h"
#include "deleteCurrentUser.h"
#include "password.h"
#include "maketoken.h"
#include "timewheel.h"


int checkToken(cmd_t *pcmd)
{
    char token[1024] = {0};
    PRINT_LINE_LOG();
    maketoken(pcmd, token);
    if (strcmp(token, pcmd->userinfo.token) == 0)
    {
        return 0;
    }
    return -1;
}
int parsingLongCommand(int netfd, cmd_t *pcmd, pathStack_t *ppathStack, MYSQL *mysql,
         threadPool_t *pthreadPool, int idx, char name[][32],int temfd ,timeWheel_t *ptimeWheel, int *findKickIdx)
{   
    PRINT_FUNCTION_LOG();
    printf("I got a commmand!\n");
    cmd_t cmd;
    memcpy(&cmd, pcmd, sizeof(cmd_t));
    train_t train;
    int cmdFlag = pcmd->cmdType;
    printf("cmdtype=%d\n", cmdFlag);
    if (cmdFlag == CMD_PUTS)
    {
        printf(" Entering puts!\n");
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            int i;
            for (i = 0; i < 1000; i++)
            {
                if (strcmp(name[i], pcmd->userinfo.username) == 0)
                {
                    break;
                }
            }
            train.flag = TRAIN_FILE;
            send(netfd, &train, sizeof(train_t), 0);

            pthread_mutex_lock(&pthreadPool->mutex);
            PRINT_LINE_LOG();
            enQueue(netfd, &pthreadPool->taskQueue, &ppathStack[i], mysql, pcmd,ptimeWheel,findKickIdx,temfd);
            printf("I am master, I send a puts!\n");
            pthread_cond_signal(&pthreadPool->cond);
            pthread_mutex_unlock(&pthreadPool->mutex);
        }
        else
        {

            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_GETS)
    {
        printf("begin get server!\n");
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        printf(" Entering gets!\n");
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            enQueue(netfd, &pthreadPool->taskQueue, &ppathStack[i], mysql, pcmd,ptimeWheel,findKickIdx,temfd);
            printf("I am master, I send a gets!\n");
            pthread_cond_signal(&pthreadPool->cond);
            pthread_mutex_unlock(&pthreadPool->mutex);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }
}


int parsingShortCommand(int netfd, cmd_t *pcmd, pathStack_t *ppathStack, MYSQL *mysql,
                   threadPool_t *pthreadPool, int idx, char name[][32])
{
    PRINT_FUNCTION_LOG();
    printf("I got a commmand!\n");
    
    cmd_t cmd;
    memcpy(&cmd, pcmd, sizeof(cmd_t));
    train_t train;
    int cmdFlag = pcmd->cmdType;
    printf("cmdtype=%d\n", cmdFlag);

    if (cmdFlag == CMD_LOGIN)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf("im login %d\n", CMD_LOGIN);
        PRINT_LINE_LOG();
        login(&ppathStack[res], pcmd, mysql, netfd, idx, name);
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_SIGN)
    {
        printf("im login %d\n", CMD_LOGIN);
        PRINT_LINE_LOG();
        userRegister(netfd, pcmd, mysql);
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_CD)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf("im cd %d\n", CMD_CD);
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            changeDir(&ppathStack[res], netfd, pcmd, mysql);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_LS)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf("I am ls = %d\n", CMD_LS);
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            listFile(&ppathStack[res], netfd, &cmd, mysql);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_PWD)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf("I am pwd = %d\n", CMD_PWD);
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            printWokringDir(&ppathStack[res], netfd, &cmd, mysql);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_RM)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf("I am removeFile = %d\n", CMD_RM);
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            removeFile(&ppathStack[res], netfd, &cmd, mysql);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_MKDIR)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf("I am mkdir %d\n", CMD_MKDIR);
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            makeDir(&ppathStack[res], netfd, &cmd, mysql);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_RMDIR)
    {
        int i;
        for (i = 0; i < 1000; i++)
        {
            if (strcmp(name[i], pcmd->userinfo.username) == 0)
            {
                break;
            }
        }
        int res = (i == 1000 ? idx : i);
        printf(" Entering rmdir\n");
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            removedir(&ppathStack[res], netfd, &cmd, mysql);
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_QUIT)
    {
        // 退出成功需要跳出循环释放线程资源
        printf(" Entering quit!\n");
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            int ret = quitCurrentUser(netfd, &cmd);
            if (ret == 0)
                return 1;
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }

    if (cmdFlag == CMD_DELUSER)
    {
        printf(" Entering deleteuser!\n");
        PRINT_LINE_LOG();
        int ret = checkToken(pcmd);
        if (ret == 0)
        {
            PRINT_LINE_LOG();
            int ret = deleteCurrentUser(netfd, &cmd, mysql);
            if (ret = 0)
            {
                // 正常退出，已经关闭netfd
                return 2;
            }
        }
        else
        {
            char msg[] = "Not logged in yet!\n";
            PRINT_LINE_LOG();
            sendResult(msg, netfd);
        }
        PRINT_USER_LOG(pcmd);
    }
}

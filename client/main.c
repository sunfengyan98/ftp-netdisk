#include <func.h>
#include "ds.h"
#include "tcpClientInit.h"
#include "login_sign.h"
#include "epoll.h"
#include "puts_gets.h"
#include "threadFunc.h"

pthread_mutex_t mutex;
int main(int argc, char *argv[])
{
    ARGS_CHECK(argc, 3);
    char arg1[256] = {0};
    memcpy(arg1, argv[1], strlen(argv[1]));
    char arg2[256] = {0};
    memcpy(arg2, argv[2], strlen(argv[2]));

    int sockfd;
    int shortNetfd;
    cmd_t cmd;

    int ret = login_sign(&sockfd, arg1, arg2 ,&cmd, &shortNetfd);
    ERROR_CHECK(ret, -1, "login_sign");

    int epfd = epoll_create(1);
    ret = epollAdd(epfd, sockfd);
    ERROR_CHECK(ret, -1, "epollAdd");
    ret = epollAdd(epfd, STDIN_FILENO);
    ERROR_CHECK(ret, -1, "epollAdd");

    bool connectFlag = true;
    threadInfo_t threadInfo;
    bzero(&threadInfo,sizeof(threadInfo_t));
    memcpy(threadInfo.arg1 , arg1, strlen(arg1));
    memcpy(threadInfo.arg2, arg2, strlen(arg2));
    threadInfo.shortNetfd=shortNetfd;
    printf("<please input a command.>\n");
    while (1)
    {
        
        struct epoll_event readySet[2];
        int readyNum = epoll_wait(epfd, readySet, 2, -1);
        for (int i = 0; i < readyNum; ++i)
        {
            if (readySet[i].data.fd == STDIN_FILENO)
            {
                char buf[4096];
                bzero(buf, sizeof(buf));
                ssize_t sret = read(STDIN_FILENO, buf, sizeof(buf));
                buf[sret - 1] = '\0';

                bzero(&cmd.parameter1, 256);
                bzero(&cmd.parameter2, 256);

                ret = commandAnalyze(buf, &cmd);
                if (ret == 1)
                {
                    continue;
                }

                
                if (cmd.cmdType == CMD_PUTS || cmd.cmdType == CMD_GETS)
                {
                    memcpy(&threadInfo.longcmd, &cmd, sizeof(cmd_t));
                    
                    pthread_t tid;
                    ret = pthread_create(&tid, NULL, threadFunc, &threadInfo);
                    THREAD_ERROR_CHECK(ret,"pthread_create");
                    
                }else{
                    transCommand(sockfd, &cmd);
                }
            }
            else
            {
                train_t trainMsg;
                bzero(&trainMsg, sizeof(trainMsg));
                ssize_t sret = recv(sockfd, &trainMsg, sizeof(trainMsg), 0);
                if (sret == 0)
                {
                    connectFlag = false;
                    break;
                }

                if (trainMsg.flag == TRAIN_RESULT)
                {
                    printf("--------------------------------------\n");
                    printf(" %s", trainMsg.data);
                    printf("--------------------------------------\n");
                    printf("<please input a command.>\n");
                }
            }
        }
        
        if (connectFlag == false)
        {
            printf("server is disconnect.\n"); 
            break;
        }
    }
    return 0;
}

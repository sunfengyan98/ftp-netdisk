#include "worker.h"
#include "taskQueue.h"
#include "threadPool.h"
#include "path.h"
#include "timewheel.h"

int timingPipe[2];
int exitPipe[2];


void handler(int signum)
{
    printf("signum = %d\n", signum);
    int retw = write(exitPipe[1], "1", 1);
}

int main(int argc, char *argv[])
{
    // ./server 192.168.72.128 1234 4
    ARGS_CHECK(argc, 4);
    
    int ret = pipe(exitPipe);
    ERROR_CHECK(ret, -1, "pipe");
    ret =pipe(timingPipe);
    ERROR_CHECK(ret, -1, "pipe");
    // 创建进程用于服务器退出
    if (fork() != 0)
    {   
        close(exitPipe[0]);
        signal(SIGUSR1, handler);
        wait(NULL);
        printf("Parent is going to exit!\n");
        exit(0);
    }
    close(exitPipe[1]);

    // 创建进程用于计时，每隔一秒向pipe管道发信号
    if (fork() != 0)
    {
        close(timingPipe[0]);
        while (1)
        {   
            int ret = write(timingPipe[1], "1", 1);
            ERROR_CHECK(ret, -1, "write");
            sleep(1);
        }
    }
    close(timingPipe[1]);

    // 线程池初始化
    threadPool_t threadPool;
    PRINT_LINE_LOG();
    threadPoolInit(&threadPool, atoi(argv[3]));
    PRINT_LINE_LOG();
    makeWorker(&threadPool);
    // 初始化目录栈
    char name[1000][32]={0};
    pathStack_t pathStack[1000];
    bzero(&pathStack, sizeof(pathStack_t) * 1000);
    // 网络连接IP与端口初始化

    int netfd[1000];
    for (int i = 0; i < 1000; i++)
    {
        netfd[i] = -1;
    }
    int findNetfdIdx[1000];
    struct sockaddr_in client_addr[1000];
    socklen_t addr_len = sizeof(struct sockaddr_in);
    bzero(client_addr, addr_len * 1000);
    uint16_t client_port[1000] = {0};
    char ip_str[1000][INET_ADDRSTRLEN] = {0};
    int cur = 0; // 用于指向accept得到的fd存储在netfd数组哪个下标中
    int sockfd;
    PRINT_LINE_LOG();
    tcpInit(argv[1], argv[2], &sockfd);
    for (int i = 0; i < 1000; i++)
    {
        findNetfdIdx[i] = -1;
    }

    // 时间轮初始化
    timeWheel_t timeWheel;
    int findKickIdx[1000];
    bzero(&timeWheel, sizeof(timeWheel_t));
    for (int i = 0; i < 1000; i++)
    {
        findKickIdx[i] = -1;
    }
    // 设置监听
    int epfd = epoll_create(7);
    PRINT_LINE_LOG();
    epollAdd(epfd, sockfd);
    PRINT_LINE_LOG();
    epollAdd(epfd, exitPipe[0]);
    PRINT_LINE_LOG();
    epollAdd(epfd, timingPipe[0]);
    //连接数据库
    MYSQL *mysql = mysql_init(NULL);
    MYSQL *cret = mysql_real_connect(mysql, "localhost", "root", "feng", "54th", 0, NULL, 0);
    if (cret == NULL)
    {
        fprintf(stderr, "mysql_real_connect:%s\n", mysql_error(mysql));
    }

    train_t train;
    cmd_t cmd;

    while (1)
    {
        //sleep(3);
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd, readySet, 1024, -1);
        for (int i = 0; i < readyNum; ++i)
        {
            //printf("readyNum=%d\n",readyNum);
            if (readySet[i].data.fd == sockfd)
            {
                //printf("sockfd!\n");
                netfd[cur] = accept(sockfd, (struct sockaddr *)&client_addr[cur], &addr_len);
                //printf("netfd=%d\n",netfd[cur]);
                inet_ntop(AF_INET, &client_addr[cur].sin_addr, ip_str[cur], INET_ADDRSTRLEN);
                client_port[cur] = ntohs(client_addr[cur].sin_port);
                
                printf("ip=%s,port=%u\n",ip_str[cur], client_port[cur]);

                PRINT_LINE_LOG();
                epollAdd(epfd, netfd[cur]);
                findNetfdIdx[netfd[cur]] = cur;
                cur++;
            }
            else if (readySet[i].data.fd == exitPipe[0])
            {
                printf("exit!\n");
                printf("threadPool is going to exit!\n");
                pthread_mutex_lock(&threadPool.mutex);
                threadPool.exitFlag = 1;
                pthread_cond_broadcast(&threadPool.cond);
                pthread_mutex_unlock(&threadPool.mutex);
                for (int j = 0; j < threadPool.tidArr.workerNum; ++j)
                {
                    pthread_join(threadPool.tidArr.arr[j], NULL);
                }
                printf("main thread is going to exit!\n");
                exit(0);
            }
            else if (readySet[i].data.fd == timingPipe[0])
            {
                char a;
                int ret = read(timingPipe[0],&a,1);
                ERROR_CHECK(ret, -1, "read");
                //printf("timingfd!\n");
                PRINT_LINE_LOG();
                timewheelupdate_timeout(&timeWheel, findKickIdx);
            }
            else
            {
                printf("netfd!\n");
                int idx = findNetfdIdx[readySet[i].data.fd];
                printf("netfd=%d\n",netfd[idx]);
                bzero(&train, sizeof(train_t));
                int ret = recv(netfd[idx], &train, sizeof(train_t), 0);
                if (ret == 0)
                {
                    PRINT_LINE_LOG();
                    epollDel(epfd, netfd[idx]);
                    printf("Connection interrupted!\n");
                    continue;
                }
                if (train.flag == TRAIN_COMMAND)
                {
                    bzero(&cmd, sizeof(cmd_t));
                    memcpy(&cmd, train.data, sizeof(cmd_t));
                    if(cmd.cmdType==CMD_PUTS || cmd.cmdType==CMD_GETS){
                        int temfd=train.length;
                        epollDel(epfd, netfd[idx]);
                        PRINT_LINE_LOG();
                        timewheelupdate_removefd(&timeWheel, findKickIdx,temfd);
                        strcat(cmd.userinfo.netinfo.ip, ip_str[idx]);
                        cmd.userinfo.netinfo.port = client_port[idx];
                        printf("cmdtype=%d\n",cmd.cmdType);
                        printf("idx=%d\n", idx);
                        PRINT_LINE_LOG();
                        parsingLongCommand(netfd[idx], &cmd, pathStack, mysql,&threadPool,idx,name,temfd,&timeWheel, findKickIdx);
                        printf("trans start!\n");
                    }
                    else{
                        PRINT_LINE_LOG();
                        timewheelupdate_useractive(&timeWheel, findKickIdx, netfd[idx]);
                        strcat(cmd.userinfo.netinfo.ip, ip_str[idx]);
                        cmd.userinfo.netinfo.port = client_port[idx];
                        printf("cmdtype=%d\n",cmd.cmdType);
                        printf("idx=%d\n", idx);
                        PRINT_LINE_LOG();
                        parsingShortCommand(netfd[idx], &cmd, pathStack, mysql,&threadPool,idx,name);
                        printf("cmd over!\n");
                    }
                    
                }
                else
                {
                    char msg[] = "Wrong command!\n";
                    PRINT_LINE_LOG();
                    sendResult(msg, netfd[idx]);
                }
            }
        }
    }
    return 0;
}

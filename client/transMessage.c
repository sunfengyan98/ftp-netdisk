#include "ds.h"
#include "transMessage.h"
int transCommand(int sockfd, cmd_t *cmd)
{
    train_t train;
    bzero(&train, sizeof(train_t));

    train.flag = TRAIN_COMMAND;
    train.length = sizeof(cmd_t);
    memcpy(train.data, cmd, sizeof(cmd_t));
    //printf("token=%s\n",cmd->userinfo.token);
    int ret = send(sockfd, &train, sizeof(train_t), 0);
    ERROR_CHECK(ret, -1, "send in transCommand");
    //printf("send over.\n");
    return 0;
}

int transLongCommand(int filesockfd, threadInfo_t *threadInfo){
    train_t train;
    bzero(&train, sizeof(train_t));

    train.flag = TRAIN_COMMAND;
    train.length = threadInfo->shortNetfd;
    memcpy(train.data, &threadInfo->longcmd, sizeof(cmd_t));
    //printf("token=%s\n",cmd->userinfo.token);
    int ret = send(filesockfd, &train, sizeof(train_t), 0);
    ERROR_CHECK(ret, -1, "send in transLongCommand");
    printf("send over.\n");
    return 0;


}


/*
int transLoginInfo(int sockfd, cmd_t *cmd)
{
    train_t train;
    bzero(&train, sizeof(train_t));
    
    train.flag = TRAIN_LOGIN;
    train.length = sizeof(cmd_t);
    memcpy(train.data, cmd, sizeof(cmd_t));
    printf("%s\n",cmd->userinfo.username);
    printf("%s\n",cmd->userinfo.password);
    int ret = send(sockfd, &train, sizeof(train_t), 0);
    ERROR_CHECK(ret, -1, "send in transSignInfo");
    return 0;
}
int transSignInfo(int sockfd, cmd_t *cmd)
{
    train_t train;
    bzero(&train, sizeof(train_t));
    
    train.flag = TRAIN_SIGN;
    train.length = sizeof(cmd_t);
    memcpy(train.data, cmd, sizeof(cmd_t));
    printf("%s\n",cmd->userinfo.username);
    printf("%s\n",cmd->userinfo.password);
    
    int ret = send(sockfd, &train, sizeof(train_t), 0);
    ERROR_CHECK(ret, -1, "send in transSignInfo");
    return 0;
}
*/
#include "password.h"
#include "maketoken.h"
int removedir(pathStack_t *ppathStack, int netfd, cmd_t *pcmd, MYSQL *mysql)
{
    PRINT_FUNCTION_LOG();
    int curDirId = ppathStack->arr[ppathStack->top - 1];
    char res[1024] = {0};
    char cond[1024] = {0};
    sprintf(cond, "pre_id=%d and filename='%s' and username='%s' and type='d' and tomb=0",
            curDirId, pcmd->parameter1, pcmd->userinfo.username);
    printf("dirname=%s\n", pcmd->parameter1);

    mysqlCmd(mysql, "count(*)", "file", cond, res);
    printf("count_res=%d\n", atoi(res));
    if (strcmp("0", res) == 0)
    {
        char msg[] = "The directory does not exist!\n";
        sendResult(msg, netfd);
    }
    else
    {
        char sql[1024] = {0};
        sprintf(sql, "update file set tomb=1 where pre_id=%d and filename='%s' and username='%s' and type='d' and tomb=0",
                curDirId, pcmd->parameter1, pcmd->userinfo.username);
        mysql_query(mysql, sql);
    }
}

int login(pathStack_t *ppathStack, cmd_t *pcmd, MYSQL *mysql, int netfd, int idx, char name[][32])
{
    PRINT_FUNCTION_LOG();
    train_t train;
    cmd_t cmd;
    bzero(&cmd, sizeof(cmd_t));
    memcpy(&cmd, pcmd, sizeof(cmd_t));
    char ePassword[1024] = {0};
    while (1)
    {
        char res[1024] = {0};
        char cond[1024] = {0};
        sprintf(cond, "username='%s' and tomb = 0", cmd.userinfo.username);
        mysqlCmd(mysql, "count(id)", "user", cond, res);
        int num = atoi(res);
        if (num == 0)
        {
            char msg[] = "The user does not exist!\n";
            sendResult(msg, netfd);
            bzero(&train, sizeof(train_t));
            recv(netfd, &train, sizeof(train_t), 0);
            memcpy(&cmd, train.data, sizeof(cmd_t));
            continue;
        }
        else
        {
            char msg[] = "The user  exists!\n";
            printf("%s\n", msg);
            char res[1024] = {0};
            char cond[1024] = {0};
            sprintf(cond, "username='%s'", cmd.userinfo.username);
            mysqlCmd(mysql, "salt", "user", cond, res);
            bzero(&train, sizeof(train_t));
            memcpy(train.data, res, strlen(res));
            train.length = strlen(res);
            train.flag = TRAIN_LOGIN;
            send(netfd, &train, sizeof(train_t), 0);
            // 取出密文密码
            bzero(cond, 1024);
            sprintf(cond, "username='%s'", cmd.userinfo.username);
            mysqlCmd(mysql, "encryted_password", "user", cond, ePassword);
            break;
        }
    }
    while (1)
    {
        bzero(&train, sizeof(train_t));
        recv(netfd, &train, sizeof(train_t), 0);
        bzero(&cmd, sizeof(cmd_t));
        memcpy(&cmd, train.data, sizeof(cmd_t));
        if (strcmp(cmd.userinfo.password, ePassword) == 0)
        {
            char token[1024] = {0};
            maketoken(&cmd, token);
            // printf("token=%s\n",token);
            // strcat(cmd.userinfo.token,token);
            bzero(&train, sizeof(train_t));
            memcpy(cmd.userinfo.token, token, strlen(token));
            memcpy(train.data,&cmd,sizeof(cmd_t));
            train.flag = TRAIN_LOGIN;
            train.length = netfd;
            send(netfd, &train, sizeof(train_t), 0);
            printf("idx=%d\n", idx);
            bzero(name[idx], 32);
            strcat(name[idx], cmd.userinfo.username);
            break;
        }
        else
        {
            char msg[] = "Password error, please re-enter!\n";
            sendResult(msg, netfd);
        }
    }
    pathStackInit(ppathStack, &cmd, mysql);
    return 0;
}

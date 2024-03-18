#include "userLogin.h"
int userLogin(int netfd, cmd_t *cmd, int *shortNetfd){
    train_t train;
    while(1){
        printf("<login>\n");
        printf("please input your username.\n");

        bzero(cmd->userinfo.username, sizeof(cmd->userinfo.username));
        ssize_t sret = read(STDIN_FILENO, cmd->userinfo.username, sizeof(cmd->userinfo.username));
        ERROR_CHECK(sret, -1, "read");
        cmd->userinfo.username[sret - 1] = '\0';

        int ret = transCommand(netfd, cmd);
        ERROR_CHECK(ret, -1, "transSignInfo");

        
        bzero(&train, sizeof(train_t));

        recv(netfd, &train, sizeof(train), 0);
        ERROR_CHECK(ret, -1, "recv");

        if (train.flag != TRAIN_LOGIN)
        {
            printf("%s\n", train.data);
        }
        else
        {
            break;
        }
    }

    char salt[21] = {0};
    memcpy(salt, train.data, strlen(train.data));

    while (1)
    {
        char clearPassword[256] = {0};
        printf("<login>\n");
        printf("please iput your password\n");
        bzero(clearPassword, sizeof(clearPassword));
        bzero(cmd->userinfo.password, sizeof(cmd->userinfo.password));
        ssize_t sret = read(STDIN_FILENO, clearPassword, sizeof(clearPassword));
        ERROR_CHECK(sret, -1, "read");
        clearPassword[sret - 1] = '\0';

        memcpy(cmd->userinfo.password, crypt(clearPassword, salt), strlen(crypt(clearPassword, salt)));
        ERROR_CHECK(cmd->userinfo.password, NULL, "crypt");

        int ret = transCommand(netfd, cmd);
        ERROR_CHECK(ret, -1, "transSignInfo");

        bzero(&train, sizeof(train));
        recv(netfd, &train, sizeof(train_t), 0);
        ERROR_CHECK(ret, -1, "recv");
        if (train.flag != TRAIN_LOGIN)
        {
            printf("%s\n", train.data);
        }
        else
        {
            *shortNetfd=train.length;

            cmd_t temcmd;
            bzero(&temcmd,sizeof(cmd_t));
            
            memcpy(&temcmd, train.data, sizeof(cmd_t));

            memcpy(cmd->userinfo.token, temcmd.userinfo.token,strlen(temcmd.userinfo.token));
           // printf("token=%s\n",cmd->userinfo.token);
            break;
        }
    }
    return 0;
}
#include "userSign.h"

int userSign(int netfd, cmd_t *cmd){
    train_t train;
    while(1){
        printf("<sign>\n");
        printf("please input your username.\n");
        bzero(cmd->userinfo.username, sizeof(cmd->userinfo.username));
        ssize_t sret = read(STDIN_FILENO, cmd->userinfo.username, sizeof(cmd->userinfo.username));
        ERROR_CHECK(sret, -1, "read");
        cmd->userinfo.username[sret - 1] = '\0';
        
        int ret = transCommand(netfd, cmd);
        ERROR_CHECK(ret, -1, "transSignInfo");

        bzero(&train, sizeof(train));

        recv(netfd, &train, sizeof(train), 0);
        ERROR_CHECK(ret, -1, "recv");

        if (train.flag != TRAIN_SIGN)
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
    printf("salt: %s\n", salt);

    while (1)
    {
        char clearPassword[256] = {0};
        printf("<sign>\n");
        printf("please iput your password\n");
        ssize_t sret = read(STDIN_FILENO, clearPassword, sizeof(clearPassword));
        ERROR_CHECK(sret, -1, "read");
        clearPassword[sret - 1] = '\0';

        char *cryptpassword = crypt(clearPassword, salt);
        //printf("cryptpassword: %s\n", salt);
        memcpy(cmd->userinfo.password, cryptpassword, strlen(cryptpassword));
        ERROR_CHECK(cmd->userinfo.password, NULL, "crypt");

        int ret = transCommand(netfd, cmd);
        ERROR_CHECK(ret, -1, "transSignInfo");

        bzero(&train, sizeof(train));
        recv(netfd, &train, sizeof(train), 0);
        ERROR_CHECK(ret, -1, "recv");
        if (train.flag != TRAIN_SIGN)
        {
            printf("%s\n", train.data);
            return 1;
        }
        else
        {
            printf("%s\n", train.data);
            break;
        }
    }

    return 0;
}

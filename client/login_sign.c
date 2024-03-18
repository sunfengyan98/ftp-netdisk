#include "login_sign.h"

int login_sign(int *sockfd, char *arg1, char *arg2, cmd_t *cmd, int *shortNetfd)
{
    char buf[4096];
    printf("Do you login or sign a new user?\n");

    while (1)
    {
        printf("input <login/sign> to login/sigin, <quit> to quit interface.\n");
        bzero(buf, sizeof(buf));
        ssize_t sret = read(STDIN_FILENO, buf, sizeof(buf));
        ERROR_CHECK(sret, -1, "read");
        ERROR_CHECK(sret, 0, "read");
        buf[sret - 1] = '\0';

        int ret = tcpClientInit(sockfd, arg1, arg2);
        ERROR_CHECK(ret, -1, "tcpClientInit");

        bzero(cmd, sizeof(cmd_t));
        ret = commandAnalyze(buf, cmd);
        if (cmd->cmdType == CMD_LOGIN || cmd->cmdType == CMD_SIGN)
        {
            if (cmd->cmdType == CMD_SIGN)
            {
                userSign(*sockfd, cmd);
            }
            if(cmd->cmdType = CMD_LOGIN){
                break;
            }
        }else if(cmd->cmdType == CMD_QUIT){
            exit(0);
        }
    }

    printf("you have enter login interface.\n");

    userLogin(*sockfd, cmd, shortNetfd);
    
    return 0;
}

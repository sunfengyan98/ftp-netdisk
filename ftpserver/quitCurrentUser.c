#include "quitCurrentUser.h"
#include "printSyslog.h"

int quitCurrentUser(int netfd, cmd_t *cmd){
    PRINT_FUNCTION_LOG();
    train_t train;
    bzero(&train, sizeof(train));
    train.flag = TRAIN_RESULT;
    
    char *message = "you quit.";
    strcpy(train.data, message);

    train.length = strlen(train.data);
    ssize_t sret = send(netfd, &train, sizeof(train), MSG_NOSIGNAL);
    ERROR_CHECK(sret, -1, "send failed in quitCurrentUser");
    
    close(netfd);

    return 0;
}

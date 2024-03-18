#include "removeFile.h"
#include "printSyslog.h"
int removeFile(pathStack_t* ppathStack, int netfd, cmd_t *cmd, MYSQL *mysql){
    PRINT_FUNCTION_LOG();
    int curDirId=ppathStack->arr[ppathStack->top-1];
    char sql[4096];
    bzero(sql, sizeof(sql));
    printf("username='%s' and filename='%s' and pre_id=%d and type='f'\n", cmd->userinfo.username, cmd->parameter1, curDirId);
    snprintf(sql, sizeof(sql), "UPDATE file SET tomb=1 WHERE username='%s' and filename='%s' and pre_id=%d and type='f'", cmd->userinfo.username, cmd->parameter1, curDirId);
    printf("sql:%s\n",sql);
    int qret = mysql_query(mysql, sql);
    if (qret != 0) {
        fprintf(stderr, "mysql_query failed: %s\n", mysql_error(mysql));
        return -1;
    }

    train_t train;
    bzero(&train, sizeof(train));

    train.flag = TRAIN_RESULT;
    char message[1024] = {0};
    // 检查影响行数，确保至少有一行被更新
    if (mysql_affected_rows(mysql) <= 0) {
        
        sprintf(message, "file delete failed, no such a file.\n");
        train.length = strlen(message);
        memcpy(train.data, message, train.length);

        ssize_t sret = send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
        ERROR_CHECK(sret, -1, "send failed in quitCurrentUser");

        fprintf(stderr, "No rows were updated.\n");
    } else {
        
        sprintf(message, "file delete sucessful.\n");
        train.length = strlen(message);
        memcpy(train.data, message, train.length);

        ssize_t sret = send(netfd, &train, sizeof(train), MSG_NOSIGNAL);
        ERROR_CHECK(sret, -1, "send failed in quitCurrentUser");

        printf("User's tomb status was successfully updated.\n");
    }
    

    return 0;
}
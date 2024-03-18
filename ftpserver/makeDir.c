#include "makeDir.h"
#include "printSyslog.h"
int makeDir(pathStack_t* ppathStack, int netfd, cmd_t *cmd, MYSQL *mysql){
    PRINT_FUNCTION_LOG();
    printf("type= %d, p1=%s, p2=%s\n",cmd->cmdType,cmd->parameter1,cmd->parameter2);
    char path[256] = {0};
    PRINT_LINE_LOG();
    getCwd(ppathStack, path, mysql);
    int curDirId=ppathStack->arr[ppathStack->top-1];

    char sql[4096] = {0};
    sprintf(sql, "select count(*) from file where username = '%s' and filename = '%s' and pre_id = %d ", cmd->userinfo.username, cmd->parameter1, curDirId);
    mysql_query(mysql, sql);
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int file_num = atoi(row[0]);

    train_t train;
    bzero(&train, sizeof(train));
    train.flag = TRAIN_RESULT;
    if (file_num > 0)
    {
        char message[1024] = {0};
        sprintf(message, "directory create failed, name is repeated.\n");
        train.length = strlen(message);
        memcpy(train.data, message, train.length);

        ssize_t sret = send(netfd, &train, sizeof(train), MSG_NOSIGNAL);
        ERROR_CHECK(sret, -1, "send failed in quitCurrentUser");
        return 1;
    }
    

    sql[4096];
    bzero(sql, sizeof(sql));
    snprintf(sql, sizeof(sql), "insert into file value (NULL,'%s','%s',%d,'%s%s','d',NULL,0) ", cmd->parameter1, cmd->userinfo.username, curDirId, path, cmd->parameter1);

    int qret = mysql_query(mysql, sql);
    if (qret != 0) {
        fprintf(stderr, "mysql_query failed: %s\n", mysql_error(mysql));
        return -1;
    }
    if (mysql_affected_rows(mysql) <= 0){
        fprintf(stderr, "mysql insert into failed\n");
        return -1;
    }
    bzero(&train, sizeof(train));
    train.flag = TRAIN_RESULT;
    char message[1024] = {0};
    sprintf(message, "directory create sucessful.\n");
    train.length = strlen(message);
    memcpy(train.data, message, train.length);

    ssize_t sret = send(netfd, &train, sizeof(train), MSG_NOSIGNAL);
    ERROR_CHECK(sret, -1, "send failed in quitCurrentUser");

    printf("directory is successfully created.\n");

    return 0;
} 
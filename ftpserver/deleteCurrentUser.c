#include "deleteCurrentUser.h"

int deleteCurrentUser(int netfd, cmd_t *cmd, MYSQL *mysql){
    PRINT_FUNCTION_LOG();
    char sql[4096];
    bzero(sql, sizeof(sql));
    snprintf(sql, sizeof(sql), "UPDATE user SET tomb = 1 WHERE username = '%s'", cmd->userinfo.username);

    int qret = mysql_query(mysql, sql);
    if (qret != 0) {
        fprintf(stderr, "mysql_query failed: %s\n", mysql_error(mysql));
        return -1;
    }

    
    // 检查影响行数，确保至少有一行被更新
    if (mysql_affected_rows(mysql) <= 0) {
        fprintf(stderr, "No rows were updated.\n");
    } else {
        printf("User's tomb status was successfully updated.\n");
    }

    bzero(sql, sizeof(sql));
    snprintf(sql, sizeof(sql), "UPDATE files SET tomb = 1 WHERE username = '%s'", cmd->userinfo.username);


    qret = mysql_query(mysql, sql);
    if (qret != 0) {
        fprintf(stderr, "mysql_query failed: %s\n", mysql_error(mysql));
        return -1;
    }

    // 检查受影响的行数
    if (mysql_affected_rows(mysql) > 0) {
        printf("User '%s' associated file records have been marked as tomb = 1 in the database.\n", cmd->userinfo.username);
    } else {
        printf("No file records found for user '%s'.\n", cmd->userinfo.username);
    }


    train_t train;
    bzero(&train, sizeof(train));

    train.flag = TRAIN_RESULT;
    char message[1024] = {0}; 
    sprintf(message,"you delete current user sucessfully, connection has been disconnected.");
    train.length=strlen(message);
    memcpy(train.data, message, train.length);

    ssize_t sret = send(netfd, &train, sizeof(train), MSG_NOSIGNAL);
    ERROR_CHECK(sret, -1, "send failed in quitCurrentUser");

    close(netfd);

}
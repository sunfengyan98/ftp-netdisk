#include "threadPool.h"
#include "worker.h"
#include "path.h"
#include "puts_gets.h"
#include "password.h"
int mysqlCmd(MYSQL *mysql, char *p1, char *p2, char *p3, char *res);
int userRegister(int netfd, cmd_t *cmd, MYSQL *mysql)
{
    PRINT_FUNCTION_LOG();
    // 此函数根据注册函数的结果决定是否执行循环
    train_t train;
    cmd_t transcmd;
    memcpy(&transcmd, cmd, sizeof(cmd_t));
    userinfo_t info;
    bzero(&train, sizeof(train_t));
    char username[32] = {0};
    printf("name=%s\n", cmd->userinfo.username);
    int cpylen = strlen(transcmd.userinfo.username);
    memcpy(username, transcmd.userinfo.username, cpylen);
    while (1)
    {
        train_t ioTrain;
        int retname = usernameConflict(username, mysql);
        if (retname == 0)
        {
            // 表示用户名无冲突，向客户返回用户名正确；

            bzero(&ioTrain, sizeof(train_t));
            ioTrain.flag = TRAIN_SIGN;
            char retstr[100] = {0};
            strcat(retstr, "usename is ok，please input password");
            char salt[21] = {0};
            printf("msg=%s\n", retstr);
            getSalt(salt, 20); // 调用函数获取盐值字符串，字符串返回给客户端，存入用户表；
            ioTrain.length = strlen(salt);
            memcpy(ioTrain.data, salt, strlen(salt));
            printf("flag=%d,retstr =%s\n", ioTrain.flag, ioTrain.data);
            send(netfd, &ioTrain, sizeof(train_t), MSG_NOSIGNAL); // 向客户发送盐制
            train_t recvTrain;
            bzero(&recvTrain, sizeof(train_t));
            recv(netfd, &recvTrain, sizeof(train_t), 0); // 接收用户密码密文
            cmd_t recvcmd;
            bzero(&recvcmd, sizeof(cmd_t));
            memcpy(&recvcmd, recvTrain.data, sizeof(cmd_t));
            printf("passwd=%s\n", recvcmd.userinfo.password);
            // 将train.data解析成cmd_t

            bzero(&info, sizeof(userinfo_t));
            memcpy(&info, &recvcmd.userinfo, sizeof(userinfo_t));
            char passwd[256] = {0};
            memcpy(passwd, info.password, strlen(info.password));
            printf("passwd=%s\n", passwd);
            // todo 将用户名，盐值字符串，密码字符串存入用户表
            char insertUserTable[4096] = {0};
            sprintf(insertUserTable, "insert into user values(NULL,'%s','%s','%s',0)", username, salt, passwd);
            mysql_query(mysql, insertUserTable);

            // todo 在文件表加入一行，表示该用户根目录i
            char realpath[256] = {0};
            char insert_sql[4096] = {0};
            char virtualpath[256] = {0};
            strcat(virtualpath, "/");
            sprintf(insert_sql, "insert into file values(NULL,'/','%s',%d,'/','d',NULL,0)", username, -1);
            mysql_query(mysql, insert_sql);

            // 注册成功，向客户端返回提示信息
            bzero(&train, sizeof(train_t));
            train.flag = TRAIN_SIGN;
            strcat(train.data, "userRegister successfull!");
            send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
            break;
        }
        else
        {
            // 用户名冲突，向客户端返回信息，提示重新输入用户名
            bzero(&ioTrain, sizeof(train_t));
            ioTrain.flag = TRAIN_RESULT;
            char retstr[100] = {0};
            strcat(retstr, "usename is conflict，please input username again");
            ioTrain.length = strlen(retstr);
            memcpy(&ioTrain.data, retstr, strlen(retstr));
            printf("flag=%d,retstr =%s\n", ioTrain.flag, ioTrain.data);
            send(netfd, &ioTrain, sizeof(train_t), 0);
        }

        bzero(&ioTrain, sizeof(train_t));
        int ret = recv(netfd, &ioTrain, sizeof(train_t), 0);
        printf("ret=%d\n", ret);
        printf("%d\n", ioTrain.flag);

        // 从接收的train中拿到cmd，在通过cmd解析成用户名
        bzero(&transcmd, sizeof(cmd_t));
        memcpy(&transcmd, train.data, sizeof(cmd_t));
        bzero(username, 32);
        cpylen = strlen(transcmd.userinfo.username);
        memcpy(username, transcmd.userinfo.username, cpylen);
        printf("usrname from clien is %s\n", username);
    }

    char dir_path[64]={0};
    sprintf(dir_path, "/home/54th/%s", username);

    // 创建目录
    int ret = mkdir(dir_path, 0666);
    ERROR_CHECK(ret, -1, "mkdir");
    return 0;
}

int usernameConflict(char *name, MYSQL *mysql)
{
    PRINT_FUNCTION_LOG();
    char sql[4096] = {0};
    printf("name=%s\n", name);
    sprintf(sql, "select username from user where username = '%s'", name);

    mysql_query(mysql, sql);
    MYSQL_RES *res = mysql_store_result(mysql);
    unsigned int i = mysql_num_rows(res);
    if (i == 0)
    {
        return 0;
    }
    else
        return 1;
}

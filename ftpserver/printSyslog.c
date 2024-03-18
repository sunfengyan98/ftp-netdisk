#include "printSyslog.h"

// 假设有一个结构体存储客户端连接信息

// 记录客户端连接信息

void log_client_connect(const cmd_t *cmd, time_t connect_time)
{
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&connect_time));

    syslog(LOG_INFO, "[CLIENT CONNECT] Username: %s, IP: %s, port: %hu, TIME: %s",
            cmd->userinfo.username, cmd->userinfo.netinfo.ip, cmd->userinfo.netinfo.port, timestamp);
}

// 记录客户端操作记录
void log_client_operation(const cmd_t *cmd, time_t operation_time)
{
    char op_timestamp[32];
    char *cmdNameArr[] = {"cd", "ls", "pwd", "puts", "gets", "rm", "mkdir", "rmdir", "sign", "login", "quit", "deluser"};
    strftime(op_timestamp, sizeof(op_timestamp), "%Y-%m-%d %H:%M:%S", localtime(&operation_time));

    syslog(LOG_INFO, "[CLIENT OPERATION] Operation: %s %s %s, TIME: %s",
            cmdNameArr[cmd->cmdType], cmd->parameter1, cmd->parameter2, op_timestamp);
}

int printServerLogs(cmd_t *cmd)
{
    
    // 初始化syslog
    openlog(NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);

    // 假设获取到客户端连接信息并记录
    if(cmd->cmdType == CMD_LOGIN || cmd->cmdType == CMD_SIGN){
        time_t connect_time = time(NULL);
        log_client_connect(cmd, connect_time);
    }
    // 假设某个时间点发生了客户端操作，并记录
    time_t operation_time = time(NULL); // 获取当前时间作为操作时间
    log_client_operation(cmd, operation_time);
    
    closelog(); // 关闭syslog

    return 0;
}

#ifndef __PRINTSYSLOG__
#define __PRINTSYSLOG__

#include <syslog.h>
#include <func.h>
#include "ds.h"

#define PRINT_LINE_LOG(func)                                                                     \
    {                                                                                            \
        FILE *pfile = fopen("/home/54th/server/log", "a+");                                      \
        char timestamp[32];                                                                      \
        time_t operation_time = time(NULL);                                                      \
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&operation_time)); \
        fprintf(pfile, "[SERVER FUNCTION CALL][%s]", timestamp);                                 \
        fprintf(pfile, "file:%s line:%d function:%s", __FILE__, __LINE__ + 1, "func");           \
        if (strlen("func") > 0)                                                                  \
        {                                                                                        \
            fprintf(pfile, "\n");                                                                \
        }                                                                                        \
        fclose(pfile);                                                                           \
    }
#define PRINT_FUNCTION_LOG()                                \
    {                                                       \
        FILE *pfile = fopen("/home/54th/server/log", "a+"); \
        fprintf(pfile, "[%s]\n", __FUNCTION__);             \
        fclose(pfile);                                      \
    }

#define PRINT_USER_LOG(cmd)                                                                                                   \
    {                                                                                                                         \
        char *cmdNameArr[] = {"cd", "ls", "pwd", "puts", "gets", "rm", "mkdir", "rmdir", "sign", "login", "quit", "deluser"}; \
        char path[256] = {0};                                                                                                 \
        sprintf(path, "%s%s%s", "/home/54th/", cmd->userinfo.username, "/log");                                               \
        FILE *fp = fopen(path, "a+");                                                                                         \
        time_t operation_time = time(NULL);                                                                                   \
        char timestamp[32];                                                                                                   \
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&operation_time));                              \
        fprintf(fp, "[CLIENT OPERATION] [%s][%s][%u][%s][%s %s %s]",                                                          \
                timestamp, cmd->userinfo.netinfo.ip, cmd->userinfo.netinfo.port, cmd->userinfo.username,                      \
                cmdNameArr[cmd->cmdType], cmd->parameter1, cmd->parameter2);                                                  \
        if (cmd->cmdType == CMD_LOGIN)                                                                                        \
        {                                                                                                                     \
            fprintf(fp, "[login sucessfully]");                                                                               \
        }                                                                                                                     \
        else if (cmd->cmdType == CMD_SIGN)                                                                                    \
        {                                                                                                                     \
            fprintf(fp, "[sign a new user]");                                                                                 \
        }                                                                                                                     \
        fprintf(fp, "\n");                                                                                                    \
        fclose(fp);                                                                                                           \
    }

#endif
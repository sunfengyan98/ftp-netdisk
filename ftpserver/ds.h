#ifndef __DS__
#define __DS__

#include <func.h>
#include "printSyslog.h"

enum{
    CMD_CD,
    CMD_LS,
    CMD_PWD,
    CMD_PUTS,
    CMD_GETS,
    CMD_RM,
    CMD_MKDIR,
    CMD_RMDIR,
    CMD_SIGN,
    CMD_LOGIN,
    CMD_QUIT,
    CMD_DELUSER
};

enum{
    TRAIN_COMMAND,
    TRAIN_FILE,
    TRAIN_RESULT,
    TRAIN_LOGIN,
    TRAIN_SIGN
};

typedef struct netinfo_s{
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
}netinfo_t;

typedef struct userinfo_s{
    char username[32];
    char password[256];
    char token[1024];
    netinfo_t netinfo;
}userinfo_t;

typedef struct cmd_s{
    int cmdType;
    char parameter1[256];
    char parameter2[256];
    userinfo_t userinfo;
}cmd_t;

typedef struct train_s{
    int flag;
    int length;
    char data[4000];
}train_t;

typedef struct kick_s{
    int kickfd[1000];
    int size;
}kick_t;

typedef struct timeWheel{
    kick_t kickArr[30];
    int curIdx;
}timeWheel_t;
#endif

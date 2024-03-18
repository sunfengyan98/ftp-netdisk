#ifndef __LOGIN_SIGN__
#define __LOGIN_SIGN__


#include <func.h>
#include "ds.h"
#include "commandAnalyze.h"
#include "tcpClientInit.h"
#include "userLogin.h"
#include "userSign.h"


int login_sign(int *sockfd, char *arg1, char *arg2, cmd_t *cmd, int *shortNetfd);

#endif
#ifndef __MAKEDIR__
#define __MAKEDIR__

#include "path.h"
#include <mysql/mysql.h>
#include "ds.h"
#include "printSyslog.h"

int makeDir(pathStack_t* ppathStack, int netfd, cmd_t *cmd, MYSQL *mysql);

#endif
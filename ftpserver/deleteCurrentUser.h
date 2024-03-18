#ifndef __DELCURRENTUSER__
#define __DELCURRENTUSER__

#include <mysql/mysql.h>
#include "ds.h"

#include "printSyslog.h"

int deleteCurrentUser(int netfd, cmd_t *cmd, MYSQL *mysql);

#endif
#ifndef __REMOVEFILE__
#define __REMOVEFILE__

#include <mysql/mysql.h>
#include "ds.h"
#include "path.h"

int removeFile(pathStack_t* ppathStack, int netfd, cmd_t *cmd, MYSQL *mysql);

#endif
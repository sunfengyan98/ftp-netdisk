#ifndef __puts_gets__
#define __puts_gets__
#include <func.h>
#include "path.h"

char *getfilename(char * path);
int server_gets(int netfd ,const pathStack_t *ppathStack,cmd_t *cmd,MYSQL *mysql);
int server_puts(int netfd ,const pathStack_t *ppathStack,cmd_t *cmd,MYSQL *mysql);
int recvn(int sockfd, void *buf, long total);

#endif

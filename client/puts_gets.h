#ifndef __puts_gets__
#define __puts_gets__
#include "ds.h"


char *getfilename(char * path);
int getsfiles(cmd_t *cmd,int sockfd);
int putsfiles(cmd_t *cmd,int sockfd);
int recvn(int sockfd, void *buf, long total);


#endif

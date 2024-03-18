#ifndef __PASSWORD__
#define __PASSWORD__

#include "ds.h"
#include "path.h"

int login(pathStack_t* ppathStack,cmd_t* pcmd,MYSQL* mysql,int netfd,int idx,char name[][32]);

int removedir(pathStack_t* ppathStack, int netfd, cmd_t* pcmd, MYSQL* mysql);

#endif

#ifndef __TRANMESSAGE_H__
#define __TRANMESSAGE_H__

#include <func.h>
#include "ds.h"

int transCommand(int sockfd, cmd_t *cmd);
int transLongCommand(int filesockfd, threadInfo_t *threadInfo);

// int transSignInfo(int sockfd, cmd_t *cmd);
// int transLoginInfo(int sockfd, cmd_t *cmd);

#endif

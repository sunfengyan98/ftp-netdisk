#include "tcpClientInit.h"

int tcpClientInit(int *sockfd, const char *ip, const char *port){
    *sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    int ret = connect(*sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    ERROR_CHECK(ret,-1,"connect");
    return 0;
}

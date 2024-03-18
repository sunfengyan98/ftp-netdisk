#include "tcpServerInit.h"

int tcpServerInit(const char *ip, uint16_t port, int *sockfd){
    // socket setsockopt bind listen
    *sockfd = socket(AF_INET,SOCK_STREAM,0);
    int reuse = 1;
    int ret = setsockopt(*sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    ERROR_CHECK(ret,-1,"setsockopt");
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    ret = bind(*sockfd,(struct sockaddr *)&addr,sizeof(addr));
    ERROR_CHECK(ret,-1,"bind");
    listen(*sockfd,10);
    return 0;
}
#include "getLocalAddr.h"

int getLocalAddr(int sockfd, cmd_t *cmd)
{
    struct sockaddr_in sa;
    size_t len = sizeof(struct sockaddr_in);
    int ret = getsockname(sockfd, (struct sockaddr *)&sa, (socklen_t*)&len);
    ERROR_CHECK(ret, -1, "getsockname");
    // 获取本地主机的IP地址

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sa.sin_addr, ip_str, sizeof(ip_str));
    memcpy(cmd->userinfo.netinfo.ip,"192.168.189.133", sizeof(ip_str)); 
    cmd->userinfo.netinfo.port = sa.sin_port;
    printf("Local IP:prot %s:%u\n", cmd->userinfo.netinfo.ip, cmd->userinfo.netinfo.port);

    return 0;
}
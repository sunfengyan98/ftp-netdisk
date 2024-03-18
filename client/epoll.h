#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <func.h>

int epollAdd(int epfd, int fd);
int epollDel(int epfd, int fd);


#endif


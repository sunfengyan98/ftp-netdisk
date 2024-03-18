#ifndef __WORKER__
#define __WORKER__
#include "ds.h"
#include "path.h"

typedef struct tidArr_s {
    pthread_t * arr;
    int workerNum;
} tidArr_t;

int userRegister(int netfd,cmd_t* cmd,MYSQL* mysql);
int getSalt(char* saltstr,int length);
int usernameConflict(char* name,MYSQL* mysql);//判断用户名是否已经存在
int whetherLogin(int netfd,char*ppath);
int tidArrInit(tidArr_t * ptidArr, int workerNum);
int isFileExite(char* path,int netfd);//判断文件夹是否在目录中，若不存在则向服务端返回消息
void *threadFunc(void *arg);

#endif

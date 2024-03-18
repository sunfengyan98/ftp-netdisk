#ifndef __PATH__
#define __PATH__
#include "ds.h"

typedef struct pathStack_s{
    int arr[32];
    int top;
}pathStack_t;

int mysqlCmd(MYSQL* mysql, char* p1, char* p2, char* p3,char* res);

static int stackPush(pathStack_t* ppathStack, int val);

static int stackPop(pathStack_t* ppathStack);

static int getList(int dirid, char* result,MYSQL* mysql);

int sendResult(char* data,int netfd);

int pathStackInit(pathStack_t* ppathStack,cmd_t* pcmd,MYSQL* mysql);

int getCwd(const pathStack_t* ppathStack, char* path, MYSQL* mysql);

int changeDir(pathStack_t* ppathStack, int netfd, cmd_t* pcmd, MYSQL* mysql);

int printWokringDir(const pathStack_t* ppathStack, int netfd, cmd_t* pcmd, MYSQL* mysql);

int listFile(const pathStack_t* ppathStack,int netfd, cmd_t* pcmd, MYSQL* mysql);

#endif

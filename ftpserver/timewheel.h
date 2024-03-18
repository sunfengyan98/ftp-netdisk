#ifndef __TIMEWHEEL__
#define __TIMEWHEEL__
#include "ds.h"

int timewheelupdate_timeout(timeWheel_t * ptimeWheel,int* findKickIdx);
//超时触发时间轮操作，将当前时针结合中所有netfd关闭



int timewheelupdate_useractive(timeWheel_t* ptimeWheel, int* findKickIdx,int netfd);//netfd活跃后，
//用户活跃，将该用户netfd加入当前时针对应的被踢集合


int timewheelupdate_removefd(timeWheel_t* ptimeWheel, int* findKickIdx,int netfd);//netfd活跃后，

#endif

#include "timewheel.h"
#include "ds.h"

int timewheelupdate_timeout(timeWheel_t * ptimeWheel,int* findKickIdx){
    PRINT_FUNCTION_LOG();
    //每次调此函数，时间指针后移动
    ptimeWheel->curIdx  ++;
    if(ptimeWheel->curIdx >=30){
        ptimeWheel->curIdx =0; //新的一圈开始
    }
   

    int curTimeidx = ptimeWheel->curIdx;
    //若当前节点被踢集合不为空，则关闭其中非空netfd
    if(ptimeWheel->kickArr[curTimeidx].size >0){
        for(int i=0;i<1000;i++){
            if(ptimeWheel->kickArr[curTimeidx].kickfd[i] >0){
                close(i);
                findKickIdx[i] = -1;
                ptimeWheel->kickArr[curTimeidx].size --;
                ptimeWheel->kickArr[curTimeidx].kickfd[i]=0;
            }
        }
    }
} 


int timewheelupdate_useractive(timeWheel_t* ptimeWheel, int* findKickIdx, int netfd){//netfd活跃后，
    PRINT_FUNCTION_LOG();
    //将netfd加入
    int curTimeidx;//当前时间点idx
    curTimeidx = ptimeWheel->curIdx;
    ptimeWheel->kickArr[curTimeidx].kickfd[netfd] = 1;
    ptimeWheel->kickArr[curTimeidx].size ++;
    //位图对应位设为1；
    
    //清空时间轮其他点中的netfd;
    int oldidx = findKickIdx[netfd];//oldidx为netfd上一个加入的时间点
    if(oldidx !=-1){//需要判空，避免访问非法内存
        ptimeWheel->kickArr[oldidx].kickfd[netfd] =0;
      //将前一个时间点中的netfd取出：标志位置0
        
        ptimeWheel->kickArr[oldidx].size --; 
        //从上一次的时针中对应集合size--
    }

    findKickIdx[netfd] = ptimeWheel->curIdx;
// 更新netfd活跃的时间点
}

int timewheelupdate_removefd(timeWheel_t* ptimeWheel, int* findKickIdx,int netfd){
    PRINT_FUNCTION_LOG();
    int idx = findKickIdx[netfd];
    ptimeWheel->kickArr[idx].kickfd[netfd] = 0;
    ptimeWheel->kickArr[idx].size --;
    return 0;
}




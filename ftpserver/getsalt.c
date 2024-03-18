#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ds.h"
int getSalt(char* saltstr,int length){
    PRINT_FUNCTION_LOG();
    char str[21]={0};
    int flag;
    srand(time(NULL));
    for(int i =0;i<length;i++){
        flag = rand()%3;
        switch(flag){
            case 0:
                str[i] = 'a' + rand()%26;
                break;
            case 1:
                str[i] = 'A' + rand() %26;
                break;
            case 2:
                str[i] = '0'+ rand() % 10;
                break;
            default:
                str[i] = 'x';
                break;
        }
    }
    str[0]='$';
    str[1]='6';
    str[2]= '$';
    str[19]='$';
    memcpy(saltstr,str,length);
    return 0;
     
}

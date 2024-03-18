#include "path.h" 

int mysqlCmd(MYSQL* mysql, char* p1, char* p2, char* p3,char* res){
    PRINT_FUNCTION_LOG();
    char sql[1024]={0};
    strcat(sql,"select ");
    strcat(sql,p1);
    strcat(sql," from ");
    strcat(sql,p2);
    strcat(sql," where ");
    strcat(sql,p3);
    //printf("sql=%s\n",sql);
    int qret = mysql_query(mysql,sql);
    if(qret != 0){
        printf("Error:%s\n",mysql_error(mysql));
        return -1;
    }
    MYSQL_RES* result= mysql_store_result(mysql);
    MYSQL_ROW row;
    row = mysql_fetch_row(result);
    strcat(res,row[0]);
    //printf("res=%s\n",res);
    return 0;
}


//边界条件暂默认满足
static int stackPush(pathStack_t* ppathStack, int val){
    PRINT_FUNCTION_LOG();
    ppathStack->arr[ppathStack->top]=val;
    ppathStack->top++;
    return 0;
}

//边界条件暂时默认满足
static int stackPop(pathStack_t* ppathStack){
    PRINT_FUNCTION_LOG();
    if(ppathStack->top==1){
        return -1;
    }
    ppathStack->arr[ppathStack->top-1]=0;
    ppathStack->top--;
    return 0;
}

int sendResult(char* data,int netfd){
    PRINT_FUNCTION_LOG();
    train_t train;
    int total =strlen(data);
    int cur = 0;
    while(total-cur >= 4000){
        memset(&train,0,sizeof(train_t));
        train.flag=TRAIN_RESULT;
        train.length=4000;
        memcpy(train.data,data+cur,4000);
        send(netfd,&train,sizeof(train_t),0);
        cur+=4000;
    }
    if(total>cur){
        memset(&train,0,sizeof(train_t));
        train.flag=TRAIN_RESULT;
        train.length = total - cur;
        memcpy(train.data,data+cur,train.length);
        send(netfd,&train,sizeof(train_t),0);
    }
    return 0;
}

int getCwd(const pathStack_t* ppathStack, char* path, MYSQL* mysql){
    PRINT_FUNCTION_LOG();
    //printf("top=%d,arr[0]=%d\n",ppathStack->top,ppathStack->arr[0]);
    strcat(path,"/");
    if(ppathStack->top==1) return 0;
    for(int i = 1 ; i < ppathStack->top ; i++ ){
        char cond[1024]={0};
        sprintf(cond,"id=%d",ppathStack->arr[i]);
        //printf("cond=%s\n",cond);
        char res[1024]={0};
        PRINT_LINE_LOG();
        mysqlCmd(mysql,"filename","file",cond,res);
        strcat(path,res);
        strcat(path,"/");
    }
   /* char cond[1024]={0};
    sprintf(cond,"id=%d",ppathStack->arr[ppathStack->top-1]);
    char res[1024]={0};
    mysqlCmd(mysql,"filename","file",cond,res);
    strcat(path,res);*/
    return 0;
}

static int getList(int dirid, char* result,MYSQL* mysql){
    PRINT_FUNCTION_LOG();
    //printf("dirid =%d\n",dirid);
    char sql[1024]={0};
    sprintf(sql,"select filename,type from file where pre_id=%d and tomb=0",dirid);
    int qret = mysql_query(mysql,sql);
    if(qret != 0){
        printf("Error:%s\n",mysql_error(mysql));
        return -1;
    }
    MYSQL_RES* res= mysql_store_result(mysql);
    //printf("rows=%ld\n",mysql_num_rows(res));
    //printf("fields=%d\n",mysql_num_fields(res));
    MYSQL_ROW row;
    char* cur=result;
    while((row = mysql_fetch_row(res))!=NULL){
        for(unsigned int i=0;i<mysql_num_fields(res);i++){
            //printf("%s\t",row[i]);
            sprintf(cur,"%s",row[i]);
            /*char* change=cur+strlen(row[i])+1;
            *change=' ';*/
            cur+=strlen(row[i]);
            *cur=' ';
            cur+=(20-strlen(row[i]));
        }
          *cur='\n';
          cur++;
    }
    *cur='\0';
    /*struct stat dirstat;
    while ((pdirent = readdir(dp)) != NULL) {
        char path[1024] = { 0 };
        sprintf(path, "%s/%s", realPath, pdirent->d_name);
        if(strcmp(".",pdirent->d_name)==0 || strcmp("..",pdirent->d_name)==0){
            continue;
        }
        stat(path, &dirstat);
        int c;
        switch (dirstat.st_mode & S_IFMT) {
        case S_IFBLK:  c = 'b'; break;
        case S_IFCHR:  c = 'c'; break;
        case S_IFDIR:  c = 'd'; break;
        case S_IFIFO:  c = 'p'; break;
        case S_IFLNK:  c = 'l'; break;
        case S_IFREG:  c = '-'; break;
        case S_IFSOCK: c = 's'; break;
        default:       c = '?'; break;
        }
        sprintf(cur,"%c", c);
        for(int i=0;i<3;i++){
            if(*cur=='\0'){
                *cur=' ';
            }
            cur++;
        }
        sprintf(cur,"%ld ", dirstat.st_size);
        for(int i=0;i<12;i++){
            if(*cur=='\0'){
                *cur=' ';
            }
            cur++;
        }
        struct tm* ptime;
        ptime = localtime(&dirstat.st_mtime);
        sprintf(cur,"%d月 %d日 %d:%d ", ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min);
        for(int i=0;i<18;i++){
            if(*cur=='\0'){
                *cur=' ';
            }
            cur++;
        }
        sprintf(cur,"%s", pdirent->d_name);
        cur+=strlen(pdirent->d_name);
        sprintf(cur,"\n");
        cur++;
    }
    *cur='\0';
    cur++;
    *cur='\n';*/
    return 0;
}

int pathStackInit(pathStack_t* ppathStack,cmd_t* pcmd, MYSQL* mysql){
    PRINT_FUNCTION_LOG();
    bzero(ppathStack,sizeof(pathStack_t));
    char res[1024]={0};
    char cond[1024]={0};
    sprintf(cond,"username='%s' and pre_id=-1 and filename='/' and type='d' and tomb=0 and path = '/'",
            pcmd->userinfo.username);
    PRINT_LINE_LOG();
    mysqlCmd(mysql,"id","file",cond,res);
    int id = atoi(res);
    PRINT_LINE_LOG();
    stackPush(ppathStack,id);
    return 0;
}

int changeDir(pathStack_t* ppathStack, int netfd, cmd_t* pcmd, MYSQL* mysql){
    PRINT_FUNCTION_LOG();
    char target[256]={0};
    memcpy(target,pcmd->parameter1,strlen(pcmd->parameter1));
    //printf("target = %s\n",target);
    if(strcmp("..",target)==0){
        //printf("try to cd ..\n");
        //printf("&&top=%d\n",ppathStack->top);
        PRINT_LINE_LOG();
       int ret= stackPop(ppathStack);
       if(ret == -1){
            char result[] = "Already in the root directory!\n";
            printf("%s",result);
            PRINT_LINE_LOG();
            sendResult(result,netfd);
       }
       return 0;
    }
    char res[1024]={0};
    char cond[1024]={0};
    sprintf(cond,"username='%s' and pre_id=%d and filename='%s' and type='d' and tomb=0",
            pcmd->userinfo.username,
            ppathStack->arr[ppathStack->top-1],
            target);
    PRINT_LINE_LOG();
    mysqlCmd(mysql,"count(filename)","file",cond,res);
    //printf("res=%s\n",res);
    if(strcmp("0",res)==0){
        char msg[]="The directory does not exist!\n";
        printf("%s",msg);
        PRINT_LINE_LOG();
        sendResult(msg,netfd);
    }
    else{
        bzero(res,1024);
        PRINT_LINE_LOG();
        mysqlCmd(mysql,"id","file",cond,res);
        //printf("id=%d\n",atoi(res));
        PRINT_LINE_LOG();
        stackPush(ppathStack,atoi(res));
    }
    return 0;
}

int printWokringDir(const pathStack_t* ppathStack, int netfd, cmd_t* pcmd, MYSQL* mysql){
    PRINT_FUNCTION_LOG();
    char path[256]={0};
    PRINT_LINE_LOG();
    getCwd(ppathStack,path,mysql);
    strcat(path,"\n");
    printf("cwd:%s",path);
    PRINT_LINE_LOG();
    sendResult(path,netfd);
    return 0;
}

int listFile(const pathStack_t* ppathStack, int netfd, cmd_t* pcmd, MYSQL* mysql){
    PRINT_FUNCTION_LOG();

    //printf("%s, top=%d\n",ppathStack->pathArr[ppathStack->top-1],ppathStack->top);
    char realPath[1024]={0};
    strcat(realPath,"/home/54th/netdisk3rd/");
    char result[32768]={0};
    for(int i = 0;i<32768;i++){
        result[i]=' ';
    }
    int dirid=ppathStack->arr[ppathStack->top-1];
    PRINT_LINE_LOG();
    getList(dirid,result,mysql);
    printf("%s",result);
    PRINT_LINE_LOG();
    sendResult(result,netfd);
    return 0;
}

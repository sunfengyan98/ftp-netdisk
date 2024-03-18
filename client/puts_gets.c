#include "puts_gets.h"
#include "md5.h"

int recvn(int sockfd, void *buf, long total)
{
    char *p = (char *)buf;
    long cursize = 0;
    while (cursize < total)
    {
        ssize_t sret = recv(sockfd, p + cursize, total - cursize, 0);
        if (sret == 0)
        {
            return 1;
        }
        cursize += sret;
    }
    return 0;
}

// 例：传参netdisk/usr1/file 调用此函数 返回file1
char *getfilename(char *path)
{
    int front = 0;
    int rear = strlen(path);
    char *curr = path;
    for (int i = 0; i < strlen(path); i++)
    {
        if (curr[i] == '/')
        {
            front = i;
        }
    }
    char *filename = (char *)malloc(sizeof(char) * 1024);
    strncpy(filename, &path[front], strlen(path) - front);
    return filename;
}

// 用户输入 gets /usr1/file1(网盘的虚拟路径) /home/54th/netdisk/user1/file1(客户端的真实路径)
// 调用gets()参数时传入的是客户端的真实路径
// 下载服务器传来的文件到真实路径里

int getsfiles(cmd_t *cmd, int sockfd)
{
    char *realpath = cmd->parameter2;
    // gets 下载 接收从服务端传来的文件
    train_t train;
    train.flag = 1;
    // 先接收服务端的文件检测火车
    recvn(sockfd, &train, sizeof(train_t));
    if (train.flag == 6)
    {
        printf("file not exist in server,download failed,client will quit\n");
        return -1;
    }
    else
    {
        printf("file exist in server\n");
    }
    // 收发来的文件总长度
    off_t filesize;
    recvn(sockfd, &train, sizeof(train_t));
    memcpy(&filesize, train.data, train.length);
    printf("filesize = %ld\n", filesize);
    printf("client is going to wait file...\n");
    int fd_exist = open(realpath, O_RDWR);

    // 此时存在一个文件
    // 发送断点续传火车
    off_t offset = 0;
    if (fd_exist != -1)
    {
        struct stat statbuf;
        // 设置偏移量
        fstat(fd_exist, &statbuf);
        train.flag = 11;
        train.length = sizeof(off_t);
        // 把存在的文件长度放进小火车车厢
        memcpy(train.data, &statbuf.st_size, train.length);
        printf("exist filesize = %ld\n", statbuf.st_size);
        offset += statbuf.st_size; // 若已存在文件就增加偏移量
        printf("offset has changed!=%ld\n", offset);
        send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
        off_t filesize;
        memcpy(&filesize, train.data, train.length);
        printf("request train data = %ld\n", filesize);
    }
    // 不存在文件 发送一个flag为0的火车表示不存在
    else
    {
        train.flag = 10;
        send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
    }

    int fd = open(realpath, O_CREAT | O_RDWR, 0666);
    // 接下来的函数用来接收文件
    off_t cursize = offset;
    off_t lastsize = 0;
    off_t slice = filesize / 100000;
    lseek(fd, offset, 0);
    while (1)
    {
        // 接收火车
        int sret = recv(sockfd, &train, sizeof(train_t), MSG_WAITALL);
        // printf("sret=%u\n", sret);
        if (sret == 0)
        {
            printf("server is disconnect!\n");
        }
        // 接收文件长度

        if (train.length < 4000)
        {
            printf("train length = %d\n", train.length);
        }
        if (train.length == 0)
        {
            break;
        }
        cursize += train.length;

        // 接收文件数据
        write(fd, train.data, train.length);
        if (cursize - lastsize > slice)
        {
            printf("%5.2lf%%\r", cursize * 100.0 / filesize);
            fflush(stdout);
            lastsize = cursize;
            if (cursize * 100.0 / filesize >= 100.0)
            {
                break;
            }
        }
    }
    printf("100.00\n");
    printf("client has recved from client!\n");
    close(fd);
    return 0;
}

// 用户输入 puts /home/54th/usr1/file1(客户端文件的真实路径) /user1/file1(网盘的虚拟路径)
// 调用puts()参数时传入的是客户端的真实路径
// 上传用户端真实路径下的文件到服务器
int putsfiles(cmd_t *cmd, int sockfd)
{
    char *realpath = cmd->parameter1;
    char *newfilename = cmd->parameter2;
    train_t train;
    train.flag = 1;

    // 先发送自己的文件名
    train.length = strlen(newfilename);
    memcpy(train.data, newfilename, train.length);
    send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);


    // 再传自己的md5码
    char md5sum[33];
    Compute_file_md5(realpath, md5sum);
    train.length = sizeof(md5sum);
    printf("files md5 : %s\n", md5sum);
    memcpy(train.data, md5sum, train.length);
    send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL); 
    int fd = open(realpath,O_RDWR);
    
    //接收文件重复检测火车
    recv(sockfd,&train,sizeof(train_t),MSG_NOSIGNAL);
    if(train.flag == 100){
        printf("same filename in server!\n");
        return 0;
    }
    
     
    // 再发送自己的文件长度
    struct stat statbuf;
    fstat(fd, &statbuf);
    train.length = sizeof(off_t);
    memcpy(train.data, &statbuf.st_size, train.length);
    printf("filesize :%ld\n", statbuf.st_size);
    send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
    printf("client is going to translate file to server !\n");

    // 收到客户端发来的断点续传请求火车
    recv(sockfd, &train, sizeof(train_t), 0);
    // recvn(sockfd,&train,sizeof(train_t));
    off_t cursize = 0;
    if (train.flag == 10)
    {
        printf("file not exist!\n");
    }
    else if (train.flag == 11)
    {
        memcpy(&cursize, train.data, train.length);
        printf("file has exist in server filesize =%ld\n", cursize);
    }

    bzero(&train, sizeof(train_t));
    if (statbuf.st_size >= 104857600)
    {
        printf("this is a bigfile , will use mmap to transfile!\n");
        char *p = (char *)mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        ERROR_CHECK(p, MAP_FAILED, "mmap");
        
        while (1)
        {
            train.flag = 1;
            // 若传输完毕
            if (cursize >= statbuf.st_size)
            {
                break;
            }
            if (statbuf.st_size - cursize >= 4000)
            {
                train.length = 4000;
            }
            else
            {
                train.length = statbuf.st_size - cursize;
            }
            memcpy(train.data, p + cursize, train.length);
            send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
            printf("%5.2lf%%\r", cursize*100.0/statbuf.st_size);
            fflush(stdout);
            cursize += train.length;
            bzero(&train, sizeof(train_t));
            // usleep(100);
        }
        train.length = 0;
        send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
        munmap(p, statbuf.st_size);
        close(fd);
        printf("bigfile has been translatend!\n");
        return 0;
    }
    else
    {
        lseek(fd, cursize, SEEK_SET);
        while (1)
        {
            bzero(&train, sizeof(train_t));
            // 小文件
            train.flag = 1;
            // 读文件数据4000个字节到火车data
            train.length = 4000;

            ssize_t sret = read(fd, train.data, train.length);
            if (sret == 0)
            {//传完了
             printf("100.00%%\n");
             train.length = 0;
             send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
               break;
            }
            // 获得火车长度
            train.length = sret;
            ssize_t sret1 = send(sockfd, &train, sizeof(train_t), MSG_NOSIGNAL);
            //printf("sret1=%lu ", sret1);
            if (sret1 == -1)
            {
                break;
            }
        }

        printf("file has been translated to server!\n");
        close(fd);
    }
}

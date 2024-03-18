#include "puts_gets.h"
#include "md5.h"
#include "path.h"
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

// 用户在客户端输入 gets /usr1/file1(网盘的虚拟路径) /home/54th/netdisk/user1/file1(客户端的真实路径)
// cmd ->parameter1 就是网盘的虚拟路径(/usr1/file1)
// char *front = /home/54th/netdisk/ 与虚拟路径拼接得到真实路径 /home/54th/netdisk/usr1/file1
// 从真实路径里获取服务端真实的文件名 文件大小 文件内容然后发送到客户端
// 进行2次send 和一次sendfile
int server_gets(int netfd, const pathStack_t *ppathStack, cmd_t *cmd, MYSQL *mysql)
{
    // 先获取用户名
    char *pathbase = "/home/54th/netdisk3rd/";
    train_t train;
    train.flag = 1;
    // 服务端发送文件到客户端
    // 获取在表中的假文件名
    char *fakefilename = cmd->parameter1;
    char username[32];
    memcpy(username, cmd->userinfo.username, sizeof(username));
    char realpath[256] = {0};
    strcat(realpath, pathbase);
    char virtualpath[256] = {0};
    getCwd(ppathStack, virtualpath, mysql);
    // 得到虚拟路径(不包含文件名)
    strcat(virtualpath, fakefilename);
    // 得到虚拟路径(包含文件名)
    // 通过username和虚拟路径在服务器寻找存不存在文件
    char sql[4096] = {0};
    sprintf(sql, "select count(*) from file where username = '%s' and path = '%s' and tomb = 0", username, virtualpath);
    mysql_query(mysql, sql);
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int file_num = atoi(row[0]);
    if (file_num <= 0)
    {
        train.flag = 6;
        send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL); // 6表示服务器没有文件
        return -1;
    }
    else
    {
        train.flag = 7;
        send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL); // 7表示服务器有文件
    }

    char name[4096] = {0};
    sprintf(name, "select md5 from file where username = '%s' and path = '%s' and tomb = 0", username, virtualpath);
    mysql_query(mysql, name);
    MYSQL_RES *res_name = mysql_store_result(mysql);
    MYSQL_ROW realfilename;
    realfilename = mysql_fetch_row(res_name);
    strcat(realpath, realfilename[0]);
    // 获取真实服务器路径(带文件名)
    printf("server file path :%s\n", realpath);
    // realpath 是服务端文件的真实地址
    int fd = open(realpath, O_RDWR);
    printf("fd:%d\n", fd);
    // 从服务端打开的文件描述符
    train.flag = 1;
    // 发送文件总长度
    struct stat statbuf;
    fstat(fd, &statbuf);
    train.length = sizeof(off_t);
    memcpy(train.data, &statbuf.st_size, train.length);
    printf("filesize = %ld\n", statbuf.st_size);
    send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
    printf("server has going to translate file to client!\n");

    // 收到客户端发来的断点续传请求火车
    recv(netfd, &train, sizeof(train_t), 0);
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
    // 循环发送文件
    if (statbuf.st_size >= 104857600)
    {
        // 若文件大于100M就建立映射传输
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
            if (statbuf.st_size - cursize > 4000)
            {
                train.length = 4000;
            }
            else
            {
                train.length = statbuf.st_size - cursize;
            }
            // 发送小火车长度
            memcpy(train.data, p + cursize, train.length);
            send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
            // 发送文件内容
            cursize += train.length;
            bzero(&train, sizeof(train_t));
            // usleep(100);
            // 改cursize长度
        }
        train.length = 0;
        send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
        // 表示传完了,发送空火车
        munmap(p, statbuf.st_size);
        close(fd);
        printf("bigfile has been translated!\n");
        return 0;
    }

    else
    {
        lseek(fd, cursize, SEEK_SET);
        while (1)
        {
            // 小文件
            bzero(&train, sizeof(train_t));
            train.length = 4000;
            // 读文件数据4000个字节到火车data
            ssize_t sret = read(fd, train.data, train.length);
            printf("sret=%lu ", sret);
            if (sret == 0)
            {
                break;
            }

            // 获得火车长度
            train.length = sret;
            ssize_t sret1 = send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
            if (sret1 == -1)
            {
                break;
            }
        }
        close(fd);
        printf("file has been tranlated to client");
        return 0;
    }
}

// 用户在客户端输入 puts /home/54th/user1/file1(客户端文件的真实路径) /user1/file1(网盘的虚拟路径)
// cmd ->parameter2 就是网盘的虚拟路径(/user1/file1)
// char *front = /home/54th/netdisk/ 与虚拟路径拼接得到真实路径 /home/54th/netdisk/usr1/file1
// 新建char数组存放客户端发来的文件名,再在服务器真实路径下通过文件名新建文件
// 接收文件大小建立映射 改变文件大小 然后接收客户端发来的文件
int server_puts(int netfd, const pathStack_t *ppathStack, cmd_t *cmd, MYSQL *mysql)
{
    printf("线程netf%d\n",netfd);
    // 服务端接收客户端传来的文件;
    char *pathbase = "/home/54th/netdisk3rd/";
    char username[32];
    memcpy(username, cmd->userinfo.username, sizeof(username));
    printf("username: %s\n", username);
    char realpath[256] = {0};
    strcat(realpath, pathbase);
    char virtualpath[256] = {0};
    getCwd(ppathStack, virtualpath, mysql);
    // 得到虚拟路径(不包含文件名)
    printf("virtualpath :%s\n", virtualpath);
    // 获取当前目录id
    int curdirid = ppathStack->arr[ppathStack->top - 1];
     

    // realpath 是服务端的真实地址
    train_t train;
    train.flag = 1;
    char fakefilename[256] = {0};
    
    // 先收发来的文件名
    recvn(netfd, &train, sizeof(train_t));
    memcpy(fakefilename, train.data, train.length);
    strcat(virtualpath, fakefilename);

     //查询同一个用户在同一路径下有无相同的文件
     char same_check[4096] = {0};
     sprintf(same_check,"select count(*) from file where username = '%s' and path = '%s' and tomb = 0",username,virtualpath);
    mysql_query(mysql, same_check);
    MYSQL_RES *res_same = mysql_store_result(mysql);
    MYSQL_ROW same_file;
    same_file = mysql_fetch_row(res_same);
    int same_file_nums = atoi(same_file[0]);
    printf("same nums = %d\n",same_file_nums);
    if(same_file_nums > 0 ){
        train.flag = 100;
        send(netfd,&train,sizeof(train_t),MSG_WAITALL);
        printf("server have the same file!\n");
        return 0;
    }
    else{
        train.flag =1;  
        send(netfd,&train,sizeof(train_t),MSG_WAITALL);
    }


    // 再收md5码
    bzero(&train, sizeof(train_t));
    recvn(netfd, &train, sizeof(train_t));
    char md5sum[33];
    memcpy(md5sum, train.data, sizeof(md5sum));
    printf("file md5 :%s\n", md5sum);
    char sql[4096] = {0};
    sprintf(sql, "select count(*) from file where md5 = '%s' and tomb = 0", md5sum);
    mysql_query(mysql, sql);
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int md5_exist = atoi(row[0]);
    printf("md5_exist = %d\n", md5_exist);
    // 查询数据库有没有相同md5
    strcat(realpath, md5sum);
    printf("virtualpath :%s\n", virtualpath);
    printf("realpath：%s\n", realpath);

    // 如果有相同的md5
    if (md5_exist > 0)
    {
        char insert_sql[4096] = {0};
        sprintf(insert_sql, "insert into file values(NULL,'%s','%s',%d,'%s','f','%s',0)", fakefilename, username, curdirid, virtualpath, md5sum);
        mysql_query(mysql, insert_sql);
        printf("flash ftp!\n");
        return -1;
    }
    // 若没有相同的md5码
    else
    {
        // 再收发来的文件总长度
        off_t filesize;
        recvn(netfd, &train.flag, sizeof(int));
        recvn(netfd, &train.length, sizeof(train.length));
        recvn(netfd, train.data, 4000);
        memcpy(&filesize, train.data, train.length);

        printf("filesize = %ld\n", filesize);
        printf("server is going to wait file...\n");
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
            send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
        }
        // 不存在文件 发送一个flag为10的火车表示不存在
        else
        {
            train.flag = 10;
            send(netfd, &train, sizeof(train_t), MSG_NOSIGNAL);
        }

        // 在服务端真实的本地路径创建文件,修改大小 建立映射
        int fd = open(realpath, O_CREAT | O_RDWR, 0666);
        // 接收文件
        off_t cursize = offset;
        off_t lastsize = 0;
        off_t slice = cursize % 100000;
        lseek(fd, offset, 0);
        bzero(&train, sizeof(train_t));
        while (1)
        {
            // 接收火车
            int sret = recv(netfd, &train, sizeof(train_t), MSG_WAITALL);
            // printf("sret=%u\n", sret);
            if (sret == -1)
            {
                close(fd);
                return 0;
            }
            if (sret == 0)
            {
                printf("client is disconnect!\n");
                close(fd);
                return 0;
            }

            if (train.length < 4000)
            {
                printf("train not full length = %d\n", train.length);
            }
            if (train.length == 0)
            {
                char insert_sql[4096] = {0};
                sprintf(insert_sql, "insert into file values(NULL,'%s','%s',%d,'%s','f','%s',0)", fakefilename, username, curdirid, virtualpath, md5sum);
                mysql_query(mysql, insert_sql);
                break;
            }
            cursize += train.length;
            // recvn(netfd,train.data,train.length);
            // 接收文件数据

            int ret = write(fd, train.data, train.length);
            if (cursize - lastsize > slice)
            {
                printf("%5.2lf%%\r", cursize * 100.0 / filesize);
                fflush(stdout);
                lastsize = cursize;
                // 传输完成后插入一条记录到表中;
                if (cursize * 100.0 / filesize >= 100.0)
                {
                    char insert_sql[4096] = {0};
                    sprintf(insert_sql, "insert into file values(NULL,'%s','%s',%d,'%s','f','%s',0)", fakefilename, username, curdirid, virtualpath, md5sum);
                    mysql_query(mysql, insert_sql);
                    break;
                }
            }
        }
        printf("100.00%%\n");
        printf("server has recved file from client!\n");
        close(fd);
        return 0;
    }
}

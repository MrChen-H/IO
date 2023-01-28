#include"ser.h"
//这个文件只是封装了原socket系列函数，增加了错误处理，原函数的用法即原型请查看man手册


int Socket(int family,int type,int port)
{
    int fd=socket(family,type,port);
    if(fd==-1)
    {
        perror("Socket create failed");
        exit(1);
    }
    return fd;
}

int Bind(int sockfd,const struct sockaddr *addr,socklen_t len )
{
    if(bind(sockfd,addr,len)==-1)
    {
        perror("Binding failed");
        exit(1);
    }
    return 0;
}

int Accept(int sockfd,struct sockaddr_in *addr,socklen_t *addrlen)
{
    int fd=accept(sockfd,(struct sockaddr*)addr,addrlen);
    if(fd==-1)
    {
        perror("Accept failed\n");
        exit(1);
    }
    char client_ip[BUFSIZ];
    printf("\nIP: %s\nPORT: %d\n",inet_ntop(AF_INET,&addr->sin_addr,client_ip,sizeof(client_ip)),ntohs(addr->sin_port));
    printf("Accept success\n");
    
    return fd;
}

int Listen(int sockfd,int Max_Num)
{
    if(listen(sockfd,Max_Num)==-1)
    {
        perror("Listen failed\n");
        exit(1);
    }
    return 0;
}

int Close(int fd)
{
    if(close(fd)==-1)
    {
        perror("Close failed!");
        exit(1);
    }
    return 0;
}

ssize_t Read(int fd,void *buf,size_t count)
{
    ssize_t size;

    while(1)
    {
        if((size=read(fd,buf,count))==-1)
        {
            if(errno!=EINTR)
            {
                return -1;
            }
        }
        else
        {
            break;
        }
    }   
    return size;
}

void Kill_zombie(int SGL)
{
    while(1)
    {
        if(waitpid(0,NULL,WNOHANG)<=0)
        {
            break;
        }
    }
    return ;
}


#include<stdio.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<errno.h>
#include<arpa/inet.h>
#include"ser.h"
#include<unistd.h>
#include<ctype.h>
#include<fcntl.h>
/*
作者:MrFeng
联系QQ:3597736463       欢迎交流学习
完成时间:2022.10.17
程序主要功能:Epoll实现的多路IO转接模型(边沿触发模式)

注意事项:1. 该程序只适用于Linux系统,window系统因为部分函数无法运行,所以并不能兼容
        2. 使用前请在ser.h中补上自己的服务器IP地址
        3. 该程序需要配合客户端一起使用
*/
int main()
{
    int server_fd,client_fd;
    server_fd=Socket(AF_INET,SOCK_STREAM,0);//创建套接字

    struct sockaddr_in server_addr,client_addr;//定义server地址和客户端地址并初始化
    server_addr.sin_family=AF_INET;//指定协议为IPV4
    server_addr.sin_port=htons(SERVER_PORT);//指定端口为5000
    server_addr.sin_addr.s_addr=inet_addr(SERVER_IP);//指定IP并转换成网络字节序

    int opt=1;
    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//设置端口复用
    Bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));//绑定IP和端口等信息
    Listen(server_fd,128);//设置同时处于建立连接的上限
        
    int e_fd=epoll_create(1024);//创建监听树根节点

    if(e_fd<0)
    {
        perror("Epoll Create Failde");//错误处理
        exit(-1);
    }
    struct epoll_event tep,ep[OPEN_MAX];//epoll_wait需要用的参数
    tep.data.fd = server_fd;
    tep.events = EPOLLIN | EPOLLET;//设置为边沿触发(分段读取数据,使服务器性能更佳)
    epoll_ctl(e_fd,EPOLL_CTL_ADD,server_fd,&tep);//将节点刮到树上
    socklen_t len=sizeof(client_addr);

    while(1)//主循环
    {
        int client_number=epoll_wait(e_fd,ep,OPEN_MAX,-1);//开始阻塞监听
        if(client_number<0)
        {
            perror("Epoll Wait Failde");
            exit(1);
        }      
        for(int i=0;i<client_number;i++)//对连接进行处理
        {   
            if(!(ep[i].events & EPOLLIN))
            {
                continue;//不符合条件,检查下一个连接
            }
            if(ep[i].data.fd == server_fd)//符合条件的连接，开始建立连接
            {
                client_fd=Accept(server_fd,&client_addr,&len);//连接
                fcntl(client_fd,F_SETFL,O_NONBLOCK);//设置该客户端描述符为非阻塞读
                tep.events=EPOLLIN;
                tep.data.fd=client_fd;
                epoll_ctl(e_fd,EPOLL_CTL_ADD,client_fd,&tep);//将描述符挂到监听树上
            }
            else
            {
                int sock_fd=ep[i].data.fd;
                char data[BUFSIZ];
                int read_number;
                while((read_number=Read(ep[i].data.fd,data,3))>0)//如果缓冲区还有剩余数据，则循环读取(因为设置了边缘触发,所以需要保证缓冲区数据能被读取干净)
                {
                    if(read_number==0)//读取到的字节数为0,说明对端已经关闭连接
                    {
                        char client_ip[BUFSIZ];
                            
                        epoll_ctl(e_fd,EPOLL_CTL_DEL,ep[i].data.fd,NULL);//将该描述符从监听树上摘除
                        Close(sock_fd);//释放描述符
                        printf("IP: %s  Close the connect!\n",inet_ntop(AF_INET,&client_addr.sin_addr,client_ip,sizeof(client_ip)));//打印关闭信息
                    }
                    else if(read_number<0)//小于0说明读取失败
                    {
                        perror("Read data Failde");
                        epoll_ctl(e_fd,EPOLL_CTL_DEL,ep[i].data.fd,NULL);
                        Close(sock_fd);
                    }
                    else if(read_number>0)//正常读取,开始数据处理并回写
                    {
                        for(int i=0;i<read_number;i++)
                        {
                        data[i]=toupper(data[i]);//将读取的数据转为大写
                        }
                        char str[]="\n数据写出: ";//打印写出信息
                        write(sock_fd,data,read_number);
                        write(STDOUT_FILENO,str,sizeof(str));
                        write(STDOUT_FILENO,data,read_number);
                    }
                 }
                    
            }
        }

    }

}

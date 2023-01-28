#ifndef __SER_H_
#define __SER_H_

#include<stdio.h>
#include<unistd.h> 
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<ctype.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/epoll.h>

#define OPEN_MAX        1024        //最大监听上限(可以通过更改配置文件来增加最大上限，默认1024)
#define SERVER_IP       ""          //你自己的服务器IP
#define SERVER_PORT     5000        //默认端口,可自行修改,但需要自行设置该端口的防火墙策略

int Socket(int family,int type,int port);//封装的错误处理的socket函数

int Bind(int sockfd,const struct sockaddr *addr,socklen_t len);//封装了错误处理的bind函数

int Accept(int sockfd,struct sockaddr_in *addr,socklen_t *addrlen);//封装了错误处理的accept函数

int Listen(int sockfd,int Max_Num);//封装了错误处理的listen函数

int Close(int fd);//封装了错误处理的close函数

ssize_t Read(int fd,void *buf,size_t count);//封装了错误处理的read函数

void Kill_zombie(int SGL);//用于多进程实现并发用的函数，这里没用


#endif  
/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：系统工具类头文件
  */ 
#ifndef SYSTOOLS_H_
#define SYSTOOLS_H_



#include "lcwftpd_headfile.h"


class systools
{
public:
	//由主机名获取ip地址
	int getlocalip(char* ip);
	//tcp服务器
	int tcp_server(const char* host,unsigned short port);
	//带超时accept
	int accept_timeout(int fd,struct sockaddr_in* addr,unsigned int wait_seconds);
	//向fd写入定长的数据
	ssize_t writen(int fd, const void* buf, size_t count);
	//从sockfd读取一行到buf
	ssize_t readline(int sockfd, void* buf, size_t maxline);

private:
	//读取数据到缓冲区buf但是不清除读取缓冲区
	ssize_t recv_peek(int sockfd, void* buf, size_t len);// recv()只能读写套接字，而不能是一般的文件描述符  
	//从fd读取定长的数据
	ssize_t readn(int fd, void* buf, size_t count);
};

#endif
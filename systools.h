//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief： 系统工具类头文件
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
};

#endif
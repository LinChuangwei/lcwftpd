//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief： ftp服务进程相关处理函数


#ifndef FTPPROTO_H_
#define FTPPROTO_H_

#include "lcwftpd_headfile.h"
#include "systools.h"
#include "ftpcodes.h"

class ftpproto
{
public:
	void handle_ftp(session_t* sess);//ftp服务进程处理函数

private:
	int ret;//用来保存返回值
    systools lcw_systools;//系统工具类
    void ftp_reply(session_t* sess,int status,const char* text);//应答函数
};


#endif

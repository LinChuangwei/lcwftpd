//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief： 子进程开启的一个会话，包含nobody进程和ftp服务进程

#ifndef SESSION_H_
#define SESSION_H_ 

#include "lcwftpd_headfile.h"
#include "ftpproto.h"
#include "nobody.h"



class session
{
public:
	void begin_session(session_t* sess);//开启一个会话
private:
	pid_t pid;//进程pid
    nobody lcw_nobody;//定义处理nobody进程的对象
    ftpproto lcw_ftpproto;//定义处理ftp服务进程的对象
};


#endif
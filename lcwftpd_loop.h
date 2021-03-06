/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：主流程类头文件
 */
#ifndef LCWFTPD_LOOP_H_
#define LCWFTPD_LOOP_H_
#include "lcwftpd_headfile.h"
#include "systools.h"
#include "session.h"
#include "parseconfig.h"

class lcwftpd_loop
{
public:
	 lcwftpd_loop();
	 ~lcwftpd_loop();
     void lcwftpd_init();
	 void lcwftpd_run();
private:
	 parseconfig& lcwparseconf;//配置文件类
	 systools lcw_systools;//系统工具类
	 session lcw_session;//会话类
	 int listenfd;//监听套接字 
	 int conn;//已连接套接字
	 pid_t pid;//进程pid
	 char* char_ip;//用于保存字符串ip
	 session_t lcw_sess;//定义一个会话结构体
};

#endif


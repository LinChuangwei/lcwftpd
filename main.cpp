/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：main函数
  */ 
#include "lcwftpd_headfile.h"
#include "lcwftpd_loop.h"


int main(int argc, char const *argv[])
{
	LCWFTPD_LOG(DEBUG,"lcwftpd by LinChuangwei.");//打印
	//子进程退出会向发进程发送SIGCHLD信号
	//SIG_IGN表示忽略SIGCHLD那个注册的信号
	signal(SIGCHLD,SIG_IGN);//可以避免僵尸进程
	lcwftpd_loop ftploop;//主任务类

	ftploop.lcwftpd_init();//ftp服务器初始化
	ftploop.lcwftpd_run();//开启ftp的循环
	return 0;
}
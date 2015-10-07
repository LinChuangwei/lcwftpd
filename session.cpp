/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：开启会话实现文件
  */ 

#include "session.h"
/**
 *begin_session - 开启一个会话
 *@sess:会话结构体
 */
void session::begin_session(session_t* sess)//开启一个会话
{
	pid = fork();//创建进程
	if (pid < 0)
	{
		LCWFTPD_LOG(ERROR,"fork int begin_session");
	}
	else if (0 == pid)//子进程是ftp服务进程
	{
		 lcw_ftpproto.handle_ftp(sess);
	}
	else//父进程是nobody进程
	{
		lcw_nobody.handle_nobody(sess);
	}
}
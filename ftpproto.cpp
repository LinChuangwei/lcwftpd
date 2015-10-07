//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief： ftp服务进程处理实现文件

#include "ftpproto.h"
#include "systools.h"
/**
 *handle_ftp - ftp服务进程的处理函数
 *@sess:会话结构体
 *主要完成从ftp客户端读取命令，解析并调用响应函数执行
 */
void ftpproto::handle_ftp(session_t* sess)
{
	LCWFTPD_LOG(DEBUG,"Hello handle_ftp");
	ftp_reply(sess,FTP_GREET,"(lcwftpd 1.0 welcome you!!!by LinChuangwei.)");
	while(1)
	{
	    //三个保存命令的数组清空
		memset(sess->cmdline,0,sizeof(sess->cmdline));
		memset(sess->cmd,0,sizeof(sess->cmd));
		memset(sess->arg,0,sizeof(sess->arg));
	}
}
/**
 *ftp_reply - ftp应答函数
 *@sess：会话结构体
 *@status：响应码
 *@text：响应文本
 */
void ftpproto::ftp_reply(session_t* sess,int status,const char* text)
{
	char buf[1024] = {0};
	sprintf(buf,"%d %s\r\n",status,text);
	lcw_systools.writen(sess->ctrl_fd,buf,strlen(buf));//进行响应
}
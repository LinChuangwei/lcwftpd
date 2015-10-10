/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：ftp主流程类实现文件
 */
#include "lcwftpd_loop.h"

/**
 *lcwftpd_loop - 构造函数，完成配置文件类初始化
 */
lcwftpd_loop::lcwftpd_loop():lcwparseconf(parseconfig::instance()),listenfd(0),conn(0)
{

}

lcwftpd_loop::~lcwftpd_loop()
{

}
/*
typedef struct ftp_session
{ 
     //控制连接
    uid_t uid;//用户id
	int ctrl_fd;//已连接套接字
    char ip[16];//ip
    char cmdline[MAX_COMMAND_LINE];//命令行
    char cmd[MAX_COMMAND];//命令
    char arg[MAX_ARG];//参数

}session_t;
*/



/**
 *lcwftpd_init - ftp服务器初始化
 */
void lcwftpd_loop::lcwftpd_init()
{
	if (getuid() != 0)//如果不是root用户启动，则会退出
	{
		LCWFTPD_LOG(ERROR,"lcwftpd must be started as root");
	}
	//加载配置文件，完成初始化
	lcwparseconf.loadfile();
	//初始化会话结构体
	lcw_sess =
	{ 
		//控制连接
        0,-1,{0},{0},{0},{0}		
	};
}	
/**
 *lcwftpd_run - ftp服务器执行函数
 */
void lcwftpd_loop::lcwftpd_run()
{
	//创建一个tcp的服务器
	listenfd = lcw_systools.tcp_server(NULL,21);
	LCWFTPD_LOG(DEBUG,"tcp_server 's listenfd:%d",listenfd);
	struct sockaddr_in addr;//用来保存对等方的地址
	while(1)
	{
		//建立连接
	    if ((conn = lcw_systools.accept_timeout(listenfd,&addr,0)) == -1)
	    {//失败
	    	LCWFTPD_LOG(ERROR,"accept_timeout");
	    }
	    //连接成功后记下ip,ip可以是整数也可以是字符串
	    //unsigned int ip = addr.sin_addr.s_addr;
	    //转换成点分十进制的字符串，方便打印。。。。。
	    char_ip = inet_ntoa(addr.sin_addr);
	    LCWFTPD_LOG(DEBUG,"%s is connecting",char_ip);
	    pid = fork();//创建进程
	    if (-1 == pid)
	    {//fork失败
	    	LCWFTPD_LOG(ERROR,"fork int lcwftpd_run");
	    }
	    else if (0 == pid)
	    {//子进程
	    	close(listenfd);//子进程无需处理监听
	    	//登记已连接的套接字
	    	lcw_sess.ctrl_fd = conn;
	    	strcpy(lcw_sess.ip,char_ip);//拷贝ip
	    	LCWFTPD_LOG(DEBUG,"子进程");
	    	//子进程开启一个新的会话
	    	lcw_session.begin_session(&lcw_sess);
	    }
	    else
	    {//父进程
	    	close(conn);//父进程不需要处理连接，关闭已连接套接字继续等待其他客户端的连接
	    	LCWFTPD_LOG(DEBUG,"父进程");
	    }
	}
	
	return;
}
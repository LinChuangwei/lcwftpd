/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：ftp服务进程相关处理函数
 */

#ifndef FTPPROTO_H_
#define FTPPROTO_H_

#include "lcwftpd_headfile.h"
#include "systools.h"
#include "ftpcodes.h"
#include "ftpstr.h"


class ftpproto
{
public:
    typedef void (ftpproto::*cmd_handler)(session_t* sess);//函数指针，带有一个参数

	//ftp服务进程处理函数
	void handle_ftp(session_t* sess);

private:	
	//定义一个map容器用来实现命令映射 
	std::map<std::string,cmd_handler> commandsmap;
	//map容器的迭代器
	std::map<std::string,cmd_handler>::iterator it;
	//初始化map容器
	void ftp_commands_map_init();
	//用来保存返回值
	int ret;
    //系统工具类
    systools lcw_systools;
    //字符串处理工具类
    ftpstr lcw_ftpstr;
    //应答函数
    void ftp_reply(session_t* sess,int status,const char* text);

    //命令对应的函数
    void do_user(session_t* sess);
    void do_pass(session_t* sess);
};


#endif

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
	//定义一个map容器用来实现命令映射 所有对象共用
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
    //另一种形式的应答函数
    void ftp_lreply(session_t* sess,int status,const char* text);
    //创建数据连接,主动用connect，被动用accept
	int get_transfer_fd(session_t* sess);
	//列出目录列表
	int list_common(session_t* sess,int type);

	int pasv_active(session_t* sess);
	int port_active(session_t* sess);
	int get_pasv_fd(session_t* sess);
	int get_port_fd(session_t* sess);

    //命令对应的函数
    void do_user(session_t* sess);
    void do_pass(session_t* sess);
    void do_cwd(session_t* sess);
	void do_xcwd(session_t* sess);
	void do_cdup(session_t* sess);
	void do_xcup(session_t* sess);
	void do_quit(session_t* sess);
	//传输参数命令
	void do_port(session_t* sess);
	void do_pasv(session_t* sess);
	void do_type(session_t* sess);
	//服务命令
	void do_retr(session_t* sess);
	void do_stor(session_t* sess);
	void do_appe(session_t* sess);
	void do_list(session_t* sess);
	void do_nlst(session_t* sess);
	void do_rest(session_t* sess);
	void do_abor(session_t* sess);
	void do_pwd(session_t* sess);
	void do_mkd(session_t* sess);
	void do_rmd(session_t* sess);
	void do_dele(session_t* sess);
	void do_rnfr(session_t* sess);
	void do_rnto(session_t* sess);
	void do_site(session_t* sess);
	void do_syst(session_t* sess);
	void do_feat(session_t* sess);
	void do_size(session_t* sess);
	void do_stat(session_t* sess);
	void do_noop(session_t* sess);
	void do_help(session_t* sess);

}; 
    


#endif

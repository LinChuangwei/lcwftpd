/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：ftp服务进程处理实现文件
 */
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
	ftp_commands_map_init();//初始化map
	LCWFTPD_LOG(DEBUG,"All supported commands:");
	for(it = commandsmap.begin();it != commandsmap.end();it++)
	{
		std::cout << it->first <<" ";
	}
	std::cout << std::endl;
	
	while(1)
	{
	    //三个保存命令的数组清空
		memset(sess->cmdline,0,sizeof(sess->cmdline));
		memset(sess->cmd,0,sizeof(sess->cmd));
		memset(sess->arg,0,sizeof(sess->arg));
		//读取命令到cmdline
		ret = lcw_systools.readline(sess->ctrl_fd,sess->cmdline,MAX_COMMAND);
		if (-1 == ret)//读取错误
		{
			LCWFTPD_LOG(ERROR,"lcw_systools.readline");
		}
		else if (0 == ret)//等于0表示断开了连接	
		{
			LCWFTPD_LOG(ERROR,"client %s disconnect",sess->ip);
		}
		//USER scut_lcw\r\n 空格前面看成命令，之后看成参数	
		//首先要去除/r/n
		lcw_ftpstr.str_trim_crlf(sess->cmdline);
		LCWFTPD_LOG(DEBUG,"cmdline:%s",sess->cmdline);
		//根据空格分割字符串
		lcw_ftpstr.str_split(sess->cmdline,sess->cmd,sess->arg,' ');
		//将命令转换为大写
		lcw_ftpstr.str_upper(sess->cmd);

		LCWFTPD_LOG(DEBUG,"cmd:%s",sess->cmd);
		LCWFTPD_LOG(DEBUG,"arg:%s",sess->arg);

		it = commandsmap.find(sess->cmd);//根据命令查找相应的执行函数
		if (it != commandsmap.end())
		{//存在和命令相对应的函数
			// LCWFTPD_LOG(DEBUG,"执行命令");
			(this->*(it->second))(sess);//执行命令
		}
		else
		{
			// LCWFTPD_LOG(DEBUG,"没有执行命令");
			// 500
			ftp_reply(sess,FTP_BADCMD,"Sorry! Unknown command");
		}

	}
}
/**
 *ftp_commands_map_init - 初始化map容器
 */
void ftpproto::ftp_commands_map_init()
{
	LCWFTPD_LOG(DEBUG,"---------ftp_commands_map_init----------");
	//访问控制命令
  	commandsmap["USER"] = &ftpproto::do_user;
  	commandsmap["PASS"] = &ftpproto::do_pass;
  	commandsmap["CWD"] = &ftpproto::do_cwd;
  	commandsmap["XCWD"] = &ftpproto::do_xcwd;
  	commandsmap["CDUP"] = &ftpproto::do_cdup;
  	commandsmap["XCUP"] = &ftpproto::do_xcup;
  	commandsmap["QUIT"] = &ftpproto::do_quit;

  	//传输参数命令
  	commandsmap["PORT"] = &ftpproto::do_port;
  	commandsmap["PASV"] = &ftpproto::do_pasv;
  	commandsmap["TYPE"] = &ftpproto::do_type;
  	
  	//服务命令
  	commandsmap["RETR"] = &ftpproto::do_retr;
  	commandsmap["STOR"] = &ftpproto::do_stor;
  	commandsmap["APPE"] = &ftpproto::do_appe;
  	commandsmap["LIST"] = &ftpproto::do_list;
  	commandsmap["NLST"] = &ftpproto::do_nlst;
  	commandsmap["REST"] = &ftpproto::do_rest;
  	commandsmap["ABOR"] = &ftpproto::do_abor;
  	commandsmap["\377\364\377\362ABOR"] = &ftpproto::do_abor;
  	commandsmap["PWD"] = &ftpproto::do_pwd;
  	commandsmap["XPWD"] = &ftpproto::do_pwd;
  	commandsmap["MKD"] = &ftpproto::do_mkd;
  	commandsmap["XMKD"] = &ftpproto::do_mkd;
  	commandsmap["RMD"] = &ftpproto::do_rmd;
  	commandsmap["XRMD"] = &ftpproto::do_rmd;
  	commandsmap["DELE"] = &ftpproto::do_dele;
  	commandsmap["RNFR"] = &ftpproto::do_rnfr;
  	commandsmap["RNTO"] = &ftpproto::do_rnto;
  	commandsmap["SITE"] = &ftpproto::do_site;
  	commandsmap["SYST"] = &ftpproto::do_syst;
  	commandsmap["FEAT"] = &ftpproto::do_feat;
  	commandsmap["SIZE"] = &ftpproto::do_size;
  	commandsmap["STAT"] = &ftpproto::do_stat;
  	commandsmap["NOOP"] = &ftpproto::do_noop;
  	commandsmap["HELP"] = &ftpproto::do_help;

  	// LCWFTPD_LOG(DEBUG,"*********ftp_commands_map_init**********");
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

/**
 *ftp_lreply - ftp另一种形式的应答函数
 *@sess：会话结构体
 *@status：响应码
 *@text：响应文本
 */    
void ftpproto::ftp_lreply(session_t* sess,int status,const char* text)
{
	char buf[1024] = {0};
	sprintf(buf,"%d-%s\r\n",status,text);
	lcw_systools.writen(sess->ctrl_fd,buf,strlen(buf));//进行响应
}

//访问控制命令
/**
 *do_user - 发送用户命令以供登陆
 *@sess:会话结构体
 */
void ftpproto::do_user(session_t* sess)
{
	LCWFTPD_LOG(DEBUG,"do_user");
	//USER scut_lcw
	struct passwd* pw = getpwnam(sess->arg);//根据用户名获取登陆相关的结构体
	if (pw == NULL)//用户不存在	
	{
		ftp_reply(sess,FTP_LOGINERR,"Oh no~Login incorrect.");
        return;
	}
	sess->uid = pw->pw_uid;//保存用户id
	//331
	ftp_reply(sess,FTP_GIVEPWORD,"Hi~Please specify the password.");
}

/**
 *do_pass - 处理密码(密码是以字符串发过来的)
 *@sess:会话结构体
 */
void ftpproto::do_pass(session_t* sess)
{
	LCWFTPD_LOG(DEBUG,"do_pass");
	//PASS 密码
	struct passwd* pw = getpwuid(sess->uid);//根据用户id获取结构体，可以看出获取密码文件信息
	if (pw == NULL)//用户不存在	
	{//530
		ftp_reply(sess,FTP_LOGINERR,"Oh no~Login incorrect.");
        return;
	}
	struct spwd* sp = getspnam(pw->pw_name);//获取影子文件信息
	if (sp == NULL)
	{//530
		ftp_reply(sess,FTP_LOGINERR,"Oh no~Login incorrect.");
        return;
	}
	//ftp客户端发送过来的密码是明文，影子文件的密码是加密过的
	//将明文进行加密,得到一个加密的密码，影子文件的操作需要root权限
	char* encrypted = crypt(sess->arg,sp->sp_pwdp);//要加密的明文(密码)，种子密钥
	LCWFTPD_LOG(DEBUG,"crypt:%s",encrypted);
	//验证密码
	if (strcmp(encrypted,sp->sp_pwdp) != 0)//验证失败
	{//530
		ftp_reply(sess,FTP_LOGINERR,"Oh no~Login incorrect.");
        return;
	}

	// //登录成功后开启接收带外数据的功能，并注册信号
	// signal(SIGURG,handle_sigurg);
	// active_sigurg(sess->ctrl_fd);

	//登录验证成功后更改工作环境，root->所登录的用户
	// umask(tunable_local_umask);//设置默认权限
	setegid(pw->pw_gid);
	seteuid(pw->pw_uid);
	chdir(pw->pw_dir);
	// 230
	ftp_reply(sess,FTP_LOGINOK,"Hello~ Login successful,Welcome you!");
}

void ftpproto::do_cwd(session_t* sess)
{

}

void ftpproto::do_xcwd(session_t* sess)
{

}

void ftpproto::do_cdup(session_t* sess)
{

}

void ftpproto::do_xcup(session_t* sess)
{

}

void ftpproto::do_quit(session_t* sess)
{

}


//传输参数命令
/**
 *do_port - 主动模式建立数据连接
 *@sess：会话结构体
 */
void ftpproto::do_port(session_t* sess)
{
	//PORT 192,168,64,1,227,59
    //ip: 192,168,64,1 端口号(高8位和低8位)：227,59
    unsigned int v[6];
    //sscanf从字符串按照特定格式获取输入
	sscanf(sess->arg,"%u,%u,%u,%u,%u,%u",&v[2],&v[3],&v[4],&v[5],&v[0],&v[1]);
	sess->port_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));//好像可以直接使用calloc
	memset(sess->port_addr,0,sizeof(struct sockaddr_in));
	sess->port_addr->sin_family = AF_INET;
	unsigned char* p = (unsigned char*)&sess->port_addr->sin_port;
	p[0] = v[0];//端口保存到相应的字段
	p[1] = v[1];
	p = (unsigned char*)&sess->port_addr->sin_addr;
	p[0] = v[2];//ip保存到相应的字段
	p[1] = v[3];
	p[2] = v[4];
	p[3] = v[5];
   
    ftp_reply(sess,FTP_PORTOK,"PORT command successful.Consider using PASV.");
    //FTP服务进程接受PORT h1,h2,h3,h4,p1,p2
    //解析ip ,port
}

void ftpproto::do_pasv(session_t* sess)
{

}

/**
 *do_type - 转换到ASCII码模式或者二进制模式
 *@sess:会话结构体
 */
void ftpproto::do_type(session_t* sess)
{
	if(strcmp(sess->arg,"A") == 0)//ASCII码模式
	{
		sess->is_ascii = 1;
		ftp_reply(sess,FTP_TYPEOK,"Switching to ASCII mode.");
	}
	else if(strcmp(sess->arg,"I") == 0)//二进制模式
	{
		sess->is_ascii = 0;
		ftp_reply(sess,FTP_TYPEOK,"Switching to Binary mode.");
	}
	else//不认识的命令
	{// 500
		ftp_reply(sess,FTP_BADCMD,"Unrecognised TYPE command.");
	}
}


//服务命令
void ftpproto::do_retr(session_t* sess)
{

}

void ftpproto::do_stor(session_t* sess)
{

}

void ftpproto::do_appe(session_t* sess)
{

}

void ftpproto::do_list(session_t* sess)
{

}

void ftpproto::do_nlst(session_t* sess)
{

}

void ftpproto::do_rest(session_t* sess)
{

}

void ftpproto::do_abor(session_t* sess)
{

}

/**
 *do_pwd - 回复当前工作目录
 *@sess:会话结构体
 */
void ftpproto::do_pwd(session_t* sess)
{
	char text[1024] = {0};
	char dir[1024+1] = {0};
	//获取当前工作目录
	getcwd(dir,1024);
	sprintf(text,"\"%s\"",dir);
	//257
	ftp_reply(sess,FTP_PWDOK,text);
}

void ftpproto::do_mkd(session_t* sess)
{

}

void ftpproto::do_rmd(session_t* sess)
{

}

void ftpproto::do_dele(session_t* sess)
{

}

void ftpproto::do_rnfr(session_t* sess)
{

}

void ftpproto::do_rnto(session_t* sess)
{

}

void ftpproto::do_site(session_t* sess)
{

}

/**
 *do_syst - 回复一些有关操作系统的信息
 *@sess:会话结构体
 */
void ftpproto::do_syst(session_t* sess)
{//215
	ftp_reply(sess,FTP_SYSTOK,"UNIX Type:L8 By LinChuangwei");
}

/**
 *do_feat - 回复服务器的特定
 *@sess:会话结构体
 */
void ftpproto::do_feat(session_t* sess)
{//211
	ftp_lreply(sess,FTP_FEAT,"Features:");
	lcw_systools.writen(sess->ctrl_fd," EPRT\r\n",strlen(" EPRT\r\n"));
	lcw_systools.writen(sess->ctrl_fd," EPSV\r\n",strlen(" EPSV\r\n"));
	lcw_systools.writen(sess->ctrl_fd," NDTH\r\n",strlen(" NDTH\r\n"));
	lcw_systools.writen(sess->ctrl_fd," PASV\r\n",strlen(" PASV\r\n"));
	lcw_systools.writen(sess->ctrl_fd," REST STREAM\r\n",strlen(" REST STREAM\r\n"));
	lcw_systools.writen(sess->ctrl_fd," SIZE\r\n",strlen(" SIZE\r\n"));
	lcw_systools.writen(sess->ctrl_fd," TVFS\r\n",strlen(" TVFS\r\n"));
	lcw_systools.writen(sess->ctrl_fd," UTF8\r\n",strlen(" UTF8\r\n"));
	ftp_reply(sess,FTP_FEAT,"End\r\n");
}

void ftpproto::do_size(session_t* sess)
{

}

void ftpproto::do_stat(session_t* sess)
{

}

void ftpproto::do_noop(session_t* sess)
{

}

void ftpproto::do_help(session_t* sess)
{

}

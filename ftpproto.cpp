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
	LCWFTPD_LOG(DEBUG,"打印一下命令");
	for(it = commandsmap.begin();it != commandsmap.end();it++)
	{
		std::cout << it->first <<std::endl;
	}
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
			LCWFTPD_LOG(DEBUG,"执行命令");
			(this->*(it->second))(sess);//执行命令
		}
		else
		{
			LCWFTPD_LOG(DEBUG,"没有执行命令");
			while(1);
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
  	commandsmap["USER"] = &ftpproto::do_user;
  	commandsmap["PASS"] = &ftpproto::do_pass;

  	LCWFTPD_LOG(DEBUG,"*********ftp_commands_map_init**********");
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
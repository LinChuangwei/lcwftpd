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
		LCWFTPD_LOG(DEBUG,"ctrl_fd:%d",sess->ctrl_fd);
		LCWFTPD_LOG(DEBUG,"ret:%d",ret);
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
  	commandsmap["XCWD"] = &ftpproto::do_cwd;
  	commandsmap["CDUP"] = &ftpproto::do_cdup;
  	commandsmap["XCUP"] = &ftpproto::do_cdup;
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

/**
 *do_cwd - 改变当前路径
 *@sess:会话结构体
 */
void ftpproto::do_cwd(session_t* sess)
{
	//更改路径,此时路径已经存放于arg中
	if (chdir(sess->arg) < 0)//这里好像也是可以切换到root的？？？？
	{//更改路径失败
		ftp_reply(sess,FTP_FILEFAIL,"Failed to change directory.");
		return;
	}
	ftp_reply(sess,FTP_CWDOK,"Directory successful changed.");
}

/**
 *do_cdup - 返回上层目录
 *@sess:会话结构体
 */
void ftpproto::do_cdup(session_t* sess)
{
	//更改到上层路径
	if (chdir("..") < 0)//这里好像也是可以切换到root的？？？？
	{//更改路径失败
		ftp_reply(sess,FTP_FILEFAIL,"Failed to change directory.");
		return;
	}
	ftp_reply(sess,FTP_CWDOK,"Directory successful changed.");
}


/**
 *do_quit - 断开
 *@sess：会话结构体
 */
void ftpproto::do_quit(session_t* sess)
{
	ftp_reply(sess,FTP_GOODBYE,"Goodbye");
	LCWFTPD_LOG(ERROR,"QUIT!!!");
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
	sess->port_addr = (struct sockaddr_in*)calloc(1,sizeof(struct sockaddr_in));//calloc自动初始化为0
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

/**
 *do_pasv - 被动模式建立数据连接,这里只是定义了一个tcp服务器，没有accept
 *@sess：会话结构体
 */
void ftpproto::do_pasv(session_t* sess)
{
	//被动模式，创建套接字，绑定端口，监听
	char ip[16] = {0};
	lcw_systools.getlocalip(ip);//获取本地IP

	sess->pasv_listen_fd = lcw_systools.tcp_server(ip,0);//0表示端口任意
	LCWFTPD_LOG(DEBUG,"pasv_listen_fd:%d",sess->pasv_listen_fd);
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if ((getsockname(sess->pasv_listen_fd,(struct sockaddr*)&addr,&addrlen)) < 0)//获取本地的地址信息，套接字必须已知
	{	
		 LCWFTPD_LOG(DEBUG,"getsockname");
	}
	//这里端口任意的话，可能被防火墙关了
	unsigned short port = ntohs(addr.sin_port);//保存端口信息，网络字节序转为主机字节序
	unsigned int v[4];
	sscanf(ip,"%u.%u.%u.%u",&v[0],&v[1],&v[2],&v[3]);
	char text[1024] = {0};
	sprintf(text,"Entering Passive Mode (%u,%u,%u,%u,%u,%u).",v[0],v[1],v[2],v[3],port>>8,port&0xFF);//port是两个字节，这里分别获取高八位和低八位
	ftp_reply(sess,FTP_PASVOK,text);
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
/**
 *do_retr -  下载文件，断点续载
 *@sess：会话结构体
 */
void ftpproto::do_retr(session_t* sess)
{
	//创建数据连接,主动用connect，被动用accept
	if (get_transfer_fd(sess))//连接失败
	{//get_transfer_fd里面会开启数据通道的传输闹钟，等下传输结束记得关掉
		 LCWFTPD_LOG(DEBUG,"get_transfer_fd(sess)");
		 return;
	}
	 LCWFTPD_LOG(DEBUG,"data_fd int do_retr:%d",sess->data_fd);
	//保存断点
	long long offset = sess->restart_pos;
	sess->restart_pos = 0;//断点位置为0

	int fd = open(sess->arg,O_RDONLY);//以只读方式打开文件
	if (-1 == fd)
	{
		//打开失败
		ftp_reply(sess,FTP_FILEFAIL,"Failed to open file.");
		return;
	}
	int ret;
	//加读锁
	ret = lcw_systools.lock_file_read(fd);
	if (-1 ==ret)
	{//加锁失败
		ftp_reply(sess,FTP_FILEFAIL,"Failed to open file.");
	}
	//判断是否是普通文件，设备文件是不能下载的
	struct stat sbuf;
	ret = fstat(fd,&sbuf);//将文件状态保存在sbuf中
	if (!S_ISREG(sbuf.st_mode))
	{//不是一个普通文件
		ftp_reply(sess,FTP_FILEFAIL,"Failed to open file.");
		return;
	}
	
	if (offset != 0)//如果有断点
	{
		ret = lseek(fd,offset,SEEK_SET);//定位断点
		if (-1 == ret)
		{
			//定位失败
			ftp_reply(sess,FTP_FILEFAIL,"Failed to lseek");
			return;
		}
	}
	char text[1024] = {0};
	if (sess->is_ascii)//TYPE A
	{
		 //ASCII码模式(实际上我们这里都是以二进制方式进行传输)
		sprintf(text,"Opening ASCII mode data connection for %s (%lld bytes).",
			 sess->arg,(long long)sbuf.st_size);
	}
	else//TYPE I
	{
		//二进制模式
		sprintf(text,"Opening BINARY mode data connection for %s (%lld bytes).",
			 sess->arg,(long long)sbuf.st_size);
	}
	//150响应
	ftp_reply(sess,FTP_DATACONN,text);
	//下载文件
	int flag = 0;//标志变量
	//sendfile直接在内核空间操作，不涉及拷贝，效率较高
	long long bytes_to_send = sbuf.st_size;//文件大小
	if (offset > bytes_to_send)//断点位置不对
	{
		bytes_to_send = 0;
	}
	else
	{
		bytes_to_send -= offset;//只传输断点到文件结束的大小
	}
	//开始传输
	while(bytes_to_send)
	{
		int num_this_time = bytes_to_send > 4096 ? 4096:bytes_to_send;
		ret = sendfile(sess->data_fd,fd,NULL,num_this_time);//不会返回EINTR,ret是发送成功的字节数
		if (-1 == ret)
		{
			//发送失败
			flag = 2;
			break;
		}
		bytes_to_send -= ret;//更新剩下的字节数
	}

	if (bytes_to_send == 0)
	{
		//发送成功
		flag = 0;
	}
	//关闭数据链接套接字,客户端貌似是通过判断套接字关闭从而判断数据是否接收完毕
	close(sess->data_fd);
	sess->data_fd = -1;
	//关闭文件
    close(fd);
	if (0 == flag)//成功并且没有收到abor
	{
		//226响应
		ftp_reply(sess,FTP_TRANSFEROK,"Transfer complete.");
	}
	else if (1 == flag)
	{//文件读取失败 451
		 ftp_reply(sess,FTP_BADSENDFILE,"Failure reading from local file.");
	}
	else if (2 == flag)
	{//文件发送失败 426
		ftp_reply(sess,FTP_BADSENDNET,"Failure writting to network stream.");
	}
}

/**
 *do_stor - 以STOR方式上传文件
 *@sess：会话结构体
 */
void ftpproto::do_stor(session_t* sess)
{
	upload_common(sess,0);//0表示STOR方式
}

/**
 *do_stor - 以APPE方式上传文件
 *@sess：会话结构体
 */
void ftpproto::do_appe(session_t* sess)
{
	upload_common(sess,1);//1表示APPE方式
}

/**
 *do_list - 处理列出目录列表命令
 *@sess：会话结构体
 */
void ftpproto::do_list(session_t* sess)
{
	//创建数据连接,主动用connect，被动用accept
	if (get_transfer_fd(sess))//连接失败
	{
		 LCWFTPD_LOG(DEBUG,"get_transfer_fd(sess)");
		 return;
	}
	//150响应
	ftp_reply(sess,FTP_DATACONN,"Here comes the directory listing.");
	//传输列表
	list_common(sess,1);//1表示详细清单
	//关闭数据链接套接字,客户端貌似是通过判断套接字关闭从而判断数据是否接收完毕
	close(sess->data_fd);
	sess->data_fd = -1;
	//226响应
	ftp_reply(sess,FTP_TRANSFEROK,"Directory send OK.");
}

/**
 *do_nlst - 处理列出目录列表命令，可用windows登录，命令短清单
 *@sess：会话结构体
 */
void ftpproto::do_nlst(session_t* sess)
{
	//创建数据连接,主动用connect，被动用accept
	if (get_transfer_fd(sess))//连接失败
	{
		LCWFTPD_LOG(DEBUG,"get_transfer_fd(sess)");
		 return;
	}
	//150响应
	ftp_reply(sess,FTP_DATACONN,"Here comes the directory listing.");
	//传输列表
	list_common(sess,0);//0表示短清单
	//关闭数据链接套接字,客户端貌似是通过判断套接字关闭从而判断数据是否接收完毕
	close(sess->data_fd);
	sess->data_fd = -1;
	//226响应
	ftp_reply(sess,FTP_TRANSFEROK,"Directory send OK.");
}

/**
 *do_rest - 断点续传
 *@sess:会话结构体
 */
void ftpproto::do_rest(session_t* sess)
{
	sess->restart_pos = atoll(sess->arg);//字符串转换为long long
	LCWFTPD_LOG(DEBUG,"restart_pos：%lld",sess->restart_pos);
	char text[1024] = {0};
	sprintf(text,"Restart position accepted (%lld).",sess->restart_pos);
	ftp_reply(sess,FTP_RESTOK,text);
}

/**
 *do_abor - quit是直接断开，abor只断开数据连接，控制连接不断开
 *@sess:会话结构体
 */
void ftpproto::do_abor(session_t* sess)//断开数据连接通道，这里是通过正常模式传输
{//没有数据在传输没直接回应即可
	ftp_reply(sess,FTP_ABOR_NOCONN,"No transfer to ABOR");
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

/**
 *do_mkd - 创建目录
 *@sess:会话结构体
 */
void ftpproto::do_mkd(session_t* sess)
{
	if (mkdir(sess->arg,0777) < 0)//0777为权限  0777&umask
	{//创建目录失败(没有写入权限时好像也可以写入)
		ftp_reply(sess,FTP_FILEFAIL,"Create directory operation failed.");//响应
		return;
	}
	char text[4096] = {0};
	if (sess->arg[0] == '/')//如果是绝对路径
	{
		 sprintf(text,"%s created.",sess->arg);
	}
	else//是相对路径
	{
		char dir[4096+1] = {0};
		//获取当前路径
		getcwd(dir,4096);
		if (dir[strlen(dir) - 1] == '/')//最后一个字符是否等于斜杠
		{
			sprintf(text,"%s%s created.",dir,sess->arg);
		}
		else
		{
			sprintf(text,"%s/%s created.",dir,sess->arg);
		}
	}
	ftp_reply(sess,FTP_MKDIROK,text);//响应
}

/**
 *do_rmd - 删除一个文件夹
 *@sess:会话结构体
 */
void ftpproto::do_rmd(session_t* sess)
{
	if (rmdir(sess->arg) < 0)
	{
		ftp_reply(sess,FTP_FILEFAIL,"Remove directory operation failed.");
		return;
	}
	ftp_reply(sess,FTP_RMDIROK,"Remove directory operation successful.");
}

/**
 *do_dele - 删除文件
 *@sess:会话结构体
 */
void ftpproto::do_dele(session_t* sess)
{
	//使用unlink删除文件
	if (unlink(sess->arg) < 0)
	{
		ftp_reply(sess,FTP_FILEFAIL,"Delete operation failed.");
		return;
	}
	ftp_reply(sess,FTP_DELEOK,"Delete operation successful.");
}

/**
 *do_rnfr - 文件重命名，先发送的是rnfr，要重命名的文件名
 *@sess:会话结构体
 */
void ftpproto::do_rnfr(session_t* sess)
{//保存要重命名的文件名
	sess->rnfr_name = (char*)malloc(strlen(sess->arg) + 1);
	memset(sess->rnfr_name,0,strlen(sess->arg) + 1);
	memcpy(sess->rnfr_name,sess->arg,strlen(sess->arg) + 1);
	ftp_reply(sess,FTP_RNFROK,"Ready for RNTO.");
}

/**
 *do_rnto - 文件重命名，在rnfr后接收到，改后的文件名
 *@sess:会话结构体
 */
void ftpproto::do_rnto(session_t* sess)
{
	if (sess->rnfr_name == NULL)
	{
		//之前没有收到过RNFR命令
		ftp_reply(sess,FTP_NEEDRNFR,"RNFR required first.");
		return;
	}
	//arg保存的是改后的文件名
	rename(sess->rnfr_name,sess->arg);//重命名
	ftp_reply(sess,FTP_RENAMEOK,"Rename successful.");
	free(sess->rnfr_name);//释放内存
	sess->rnfr_name = NULL;
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

/**
 *do_size - 查看文件大小
 *@sess:会话结构体
 */
void ftpproto::do_size(session_t* sess)
{
	struct stat buf;
	if (stat(sess->arg,&buf) < 0)//获取状态失败
	{
		ftp_reply(sess,FTP_FILEFAIL,"SIZE operation failed.");
		return;		 
	}
	//判定是不是一个文件
	if (!S_ISREG(buf.st_mode))
	{//不是一个普通的文件
		 ftp_reply(sess,FTP_FILEFAIL,"Could not get file size.");
		 return;
	}
	char text[1024] = {0};
	sprintf(text,"%lld",(long long)buf.st_size);
	ftp_reply(sess,FTP_SIZEOK,text);//应答文件的大小	
}

/**
 *do_stat - 查看服务器的状态
 *@sess:会话结构体
 */
void ftpproto::do_stat(session_t* sess)
{

}

/**
 *do_noop - NOOP,这个是防止空闲断开
 *@sess:会话结构体
 */
void ftpproto::do_noop(session_t* sess)
{
	ftp_reply(sess,FTP_NOOPOK,"NOOP ok.");
}

/**
 *do_help - 回复一些提示的信息
 *@sess:会话结构体
 */
void ftpproto::do_help(session_t* sess)
{
	ftp_lreply(sess,FTP_HELP, "The following commands are recognized.");
	lcw_systools.writen(sess->ctrl_fd, " ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n",
		strlen(" ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"));

	lcw_systools.writen(sess->ctrl_fd, " MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n",
		strlen(" MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"));

	lcw_systools.writen(sess->ctrl_fd, " RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n",
		strlen(" RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"));

	lcw_systools.writen(sess->ctrl_fd, " XPWD XRMD\r\n",
		strlen(" XPWD XRMDr\n"));

	ftp_reply(sess,FTP_HELP, "Help OK.");
}

/**
 *get_port_fd - 获取主动模式数据套接字
 *@sess:会话结构体
 *失败返回1，成功返回0
 */
int ftpproto::get_port_fd(session_t* sess)
{//主动模式此时ftp服务器相当于客户端
	int fd = lcw_systools.tcp_client(0);//到时改成20端口
	if (-1 == fd)
	{
		//失败
		LCWFTPD_LOG(DEBUG,"fd=-1");
		return 1;
	}
	if (lcw_systools.connect_timeout(fd,sess->port_addr,0) < 0)
	{//失败
		 close(fd);
		 return 1;
	}
	sess->data_fd = fd;//数据连接套接字
	LCWFTPD_LOG(DEBUG,"data_fd:%d",sess->data_fd);
	return 0;
}

/**
 *get_pasv_fd - 获取被动模式数据套接字
 *@sess:会话结构体
 *失败返回1，成功返回0
 */
int ftpproto::get_pasv_fd(session_t* sess)
{
	//do_pasv中定义了一个tcp服务器，完成绑定和监听，这里完成accept
	int fd = lcw_systools.accept_timeout(sess->pasv_listen_fd,NULL,0);
	close(sess->pasv_listen_fd);//监听套接字此时已经没用，关掉
	if (-1 == fd)
	{//失败
		LCWFTPD_LOG(DEBUG,"fd=-1");
		return 1;
	}
	sess->data_fd = fd;//数据连接套接字
	LCWFTPD_LOG(DEBUG,"data_fd:%d",sess->data_fd);
	return 0;
}

/**
 *port_active - 检查主动模式是否被激活过
 *@sess:会话结构体
 *激活过返回1，没有激活过返回0
 */
int ftpproto::port_active(session_t* sess)
{//若主动模式被激活过，则有保存对等的地址，port结构体不为空
	if (sess->port_addr != NULL)
	{//主动模式处于激活的状态
		if (pasv_active(sess))//被动模式也处于激活的状态
		{//这种 状况是不允许的
			LCWFTPD_LOG(ERROR,"both port and pasv are active");
		}
		return 1;
	}
	return 0;//没有激活过
}

/**
 *pasv_active - 检查被动模式是否被激活过
 *@sess:会话结构体
 *激活过返回1，没有激活过返回0
 */
int ftpproto::pasv_active(session_t* sess)
{
  //若被动模式开启，pasv_listen_fd应该不等于-1
	if (sess->pasv_listen_fd != -1)
	{
		//处于激活状态
		if (port_active(sess))
		{
			//这种 状况是不允许的
			LCWFTPD_LOG(ERROR, "both port and pasv are active");
		}
		return 1;
	}
	return 0;
}


/**
 *get_transfer_fd - 创建数据连接,主动用connect，被动用accept
 *@sess:会话结构体
 *成功返回0，失败返回1
 */
int ftpproto::get_transfer_fd(session_t* sess)
{
	//检测是否收到PORT或者PASV命令
	//激活过是1
	if (!port_active(sess) && !pasv_active(sess))//没有被激活过
	{//两个都没有被激活过，要给个应答，若直接返回会使客户端阻塞
		ftp_reply(sess,FTP_BADSENDCONN,"Use PORT or PASV first.");
		return 1;//失败
	}
	int ret = 0;
	if (port_active(sess))//主动模式，服务器创建数据套接字(bind 20端口)
	{	//调用connect连接客户端IP与端口，建立数据连接
	   if (get_port_fd(sess))//获取主动模式的套接字
	   {//失败
	   	   ret = 1;
	   }   	
	}

	if (pasv_active(sess))//被动模式，使用accept
	{
		if (get_pasv_fd(sess))//获取被动模式的套接字
	   {//失败
	   	   ret = 1;
	   }   	
	}

    if (sess->port_addr)
	{//之前调用过do_port了，数据连接用完就free
		free(sess->port_addr);
		sess->port_addr = NULL;
	}
	// if (ret)
	// {
	// 	//成功创建数据通道后就开启闹钟信号
	// 	start_data_alarm();//重新安装SIGALRM信号，并启动闹钟
	// }
	return ret;//成功是0，失败是1
}

/**
 *list_common - 列出目录列表
 *@sess:会话结构体
 *@type：要列出的目录列表的类型，1表示详细清单，0表示简要清单
 *成功返回0，失败返回1
 */
int ftpproto::list_common(session_t* sess,int type)
{
	DIR* dir = opendir(".");//打开当前目录
	if (dir == NULL)//打开失败
	{
		 return 1;
	}
	// struct dirent {
 //               ino_t          d_ino;       /* inode number */
 //               off_t          d_off;       /* not an offset; see NOTES */
 //               unsigned short d_reclen;     length of this record 
 //               unsigned char  d_type;      /* type of file; not supported
 //                                              by all file system types */
 //               char           d_name[256]; /* filename */
 //           };
	struct dirent* dt;
	struct stat sbuf;
	while((dt = readdir(dir)) != NULL)//遍历目录列表
	{
		if (dt->d_name[0] == '.')//过滤隐藏的文件
		{
			continue;
		}
		// struct stat {
  //              dev_t     st_dev;     /* ID of device containing file */
  //              ino_t     st_ino;     /* inode number */
  //              mode_t    st_mode;    /* protection */
  //              nlink_t   st_nlink;   /* number of hard links */
  //              uid_t     st_uid;     /* user ID of owner */
  //              gid_t     st_gid;     /* group ID of owner */
  //              dev_t     st_rdev;     device ID (if special file) 
  //              off_t     st_size;    /* total size, in bytes */
  //              blksize_t st_blksize; /* blocksize for file system I/O */
  //              blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
  //              time_t    st_atime;   /* time of last access */
  //              time_t    st_mtime;   /* time of last modification */
  //              time_t    st_ctime;   /* time of last status change */
  //          };
		if (lstat(dt->d_name,&sbuf) < 0)//stat获取文件的状态
		{
			 continue;
		}
		char buf[1024] = {0};
		if (type)//详细清单
		{
			//获取权限
			const char* perms = lcw_systools.statbuf_get_perms(&sbuf);
	        //格式化权限
			
			int off = 0;
			off += sprintf(buf,"%s ",perms);
			off += sprintf(buf + off," %3lu %-8d %-8d ",(unsigned long)sbuf.st_nlink,sbuf.st_uid,sbuf.st_gid);//链接数，uid，gid
			off += sprintf(buf + off,"%8lu ",(unsigned long)sbuf.st_size);//大小

			//时间的格式化
			const char* databuf = lcw_systools.statbuf_get_data(&sbuf);

			off += sprintf(buf + off,"%s ",databuf);
			if (S_ISLNK(sbuf.st_mode))//如果是符号链接文件 man lstat
			{
				char tmp[1024] = {0};
				readlink(dt->d_name,tmp,sizeof(tmp));//获取符号链接文件所指向的文件
				sprintf(buf + off,"%s -> %s\r\n",dt->d_name,tmp);//格式化文件名
			}
			else
			{
				sprintf(buf + off,"%s\r\n",dt->d_name);//格式化文件名
			}
		}
		else//短清单
		{
			sprintf(buf,"%s\r\n",dt->d_name);//格式化文件名
		}
		//printf("%s",buf);//打印查看一下
		//通过数据套接字发送目录列表
	    lcw_systools.writen(sess->data_fd,buf,strlen(buf));

	}
	closedir(dir);
	return 0;
}

/**
 *upload_common - 上传文件
 *@sess:会话结构体
 *@is_append:是否以appe方式上传，0表示STOR方式，1表示APPE方式
 */
void ftpproto::upload_common(session_t* sess,int is_append)
{
	//创建数据连接,主动用connect，被动用accept
	if (get_transfer_fd(sess))//连接失败
	{
		 LCWFTPD_LOG(DEBUG,"get_transfer_fd(sess)");
		 return;
	}
	//保存断点,REST命令保存断点
	long long offset = sess->restart_pos;
	sess->restart_pos = 0;//断点置为0
	//以创建，只写的方式打开文件，权限默认0666，实际的权限时0666 & umask
	int fd = open(sess->arg,O_CREAT | O_WRONLY ,0666);//以只读方式打开文件
	LCWFTPD_LOG(DEBUG,"file name：%s",sess->arg);//文件名是有带路径的
	if (-1 == fd)
	{
		//创建失败
		ftp_reply(sess,FTP_UPLOADFAIL,"Could not cteate file.");
		return;
	}
	int ret;
	//加写锁
	ret = lcw_systools.lock_file_write(fd);
	if (-1 ==ret)
	{
		ftp_reply(sess,FTP_UPLOADFAIL,"Could not cteate file.");
		return;
	}
	//查看下是什么方式上传
	if (!is_append && offset == 0)//STOR上传
	{
		//把原来文件的长度清零
		ftruncate(fd,0);
		if (lseek(fd,0,SEEK_SET) < 0)//文件定位到文件头的位置
		{
			ftp_reply(sess,FTP_UPLOADFAIL,"Could not cteate file.");
			return;
		} 
	}
	else if (!is_append && offset != 0)//REST+STOR 断点续传
	{
		if (lseek(fd,offset,SEEK_SET) < 0)//将读写位置指向文件头后再增加offset个位移量
		{
			ftp_reply(sess,FTP_UPLOADFAIL,"Could not cteate file.");
			return;
		}
	}
	else if (is_append)//APPE 断点续传
	{//将读写位置指向文件尾后再增加offset个位移量
		if (lseek(fd,0,SEEK_END) < 0)//偏移到文件末尾
		{
			ftp_reply(sess,FTP_UPLOADFAIL,"Could not cteate file.");
			return;
		}
	}
	//获取文件状态
	struct stat sbuf;
	ret = fstat(fd,&sbuf);//将文件状态保存在sbuf中
	if (!S_ISREG(sbuf.st_mode))
	{//不是一个普通文件
		ftp_reply(sess,FTP_UPLOADFAIL,"Could not create file.");
		return;
	}

	char text[1024] = {0};
	if (sess->is_ascii)//TYPE A
	{
		 //ASCII码模式(实际上我们这里都是以二进制方式进行传输)
		sprintf(text,"Opening ASCII mode data connection for %s (%lld bytes).",
			 sess->arg,(long long)sbuf.st_size);
	}
	else//TYPE I
	{
		//二进制模式
		sprintf(text,"Opening BINARY mode data connection for %s (%lld bytes).",
			 sess->arg,(long long)sbuf.st_size);
	}

	//150响应
	ftp_reply(sess,FTP_DATACONN,text);
	
	//上传文件
	//这个buf变大的时候传输速度可相应有所提高
	//可提供一个配置项来配置这个65536//64K
	char buf[1024];
	int flag = 0;//标志变量
	while(1)
	{
		ret = read(sess->data_fd,buf,sizeof(buf));//从数据套接字中读取数据到buf
		if (-1 == ret)
		{
			if (errno == EINTR)//被信号中断
			{
				 continue;
			}
			else
			{
				flag = 2;//失败
				break;
			}
		}
		else if (0 == ret)
		{//读取完毕
			 flag = 0;//成功
			 break;
		}
		if (lcw_systools.writen(fd,buf,ret) != ret)//写入文件
		{
			flag = 1;//失败
			break;
		} 
	}
	//关闭数据链接套接字,客户端貌似是通过判断套接字关闭从而判断数据是否接收完毕
	close(sess->data_fd);
	sess->data_fd = -1;
    //关闭文件
    close(fd);
	if (0 == flag)//成功
	{
		//226响应
		ftp_reply(sess,FTP_TRANSFEROK,"Transfer complete.");
	}
	else if (1 == flag)
	{//写入本地失败  451
		 ftp_reply(sess,FTP_BADSENDFILE,"Failure writting to local file.");
	}
	else if (2 == flag)
	{//读取网络失败  426
		ftp_reply(sess,FTP_BADSENDNET,"Failure reading from network stream.");
	}
}
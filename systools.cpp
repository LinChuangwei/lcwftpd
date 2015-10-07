//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief： 系统工具类实现

#include "systools.h"
/**
 * getlocalip - 由主机名获取ip地址
 * @ip:ip的指针
 */
int systools::getlocalip(char* ip)//暂时这样实现
{
	strcpy(ip,"192.168.64.128");
	return 0;
}
/**
 *tcp_server - tcp服务器
 *@host:主机名称或ip地址
 *@port:端口号
 *成功返回监听套接字
 */
int systools::tcp_server(const char* host,unsigned short port)
{
	int listenfd;
	if ((listenfd = socket(PF_INET,SOCK_STREAM,0)) < 0)//创建一个套接字
	{
		LCWFTPD_LOG(ERROR,"socket");
	}
	// struct sockaddr_in
	// {
	// 	short sin_family;//Addressfamily一般来说AF_INET（地址族）PF_INET（协议族）
	// 	unsigned short sin_port;//Portnumber(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)
	// 	struct in_addr sin_addr;//Internetaddress
	// 	unsigned char sin_zero[8];//Samesizeasstructsockaddr没有实际意义,只是为了跟SOCKADDR结构在内存中对齐
	// };
	//in_addr_t 一般为 32位的unsigned int
	//struct in_addr {
	//     in_addr_t s_addr;
	// 	};

	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;//协议族
	if (host != NULL)
	{//inet_aton输入如果是一个有效的ip地址，转换为网络字节序，不是返回0
		if (inet_aton(host,&servaddr.sin_addr) == 0)//如果不是一个有效的ip地址
		{//很有可能是一个主机名称
			struct hostent* hp;
			if ((hp = gethostbyname(host)) == NULL)
			{//获取对应于给定主机名的包含主机名字和地址信息的hostent结构指针
				LCWFTPD_LOG(ERROR,"gethostbyname");
			}
			servaddr.sin_addr = *(struct in_addr*)hp->h_addr;//填入ip地址
		}
	}
	else//host为空 
	{//INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	//填充端口号
	servaddr.sin_port = htons(port);
	//设置地址重复利用
	int on = 1;
	if ((setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on))) < 0)
	{
		LCWFTPD_LOG(ERROR,"setsockopt");
	}
	//绑定
	if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	{
		LCWFTPD_LOG(ERROR,"bind");
	}
	//监听
	if (listen(listenfd,SOMAXCONN) < 0)
	{
		LCWFTPD_LOG(ERROR,"listen");
	}
	return listenfd;
}

/**
 * accept_timeout - 接收连接超时函数，利用select实现 
 * @fd:文件描述符
 * @addr:输出参数，返回对方的地址
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时秒数
 * 成功（未超时）返回已连接套接字，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int systools::accept_timeout(int fd,struct sockaddr_in* addr,unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	if (wait_seconds > 0)//wait_seconds大于0才检测超时
	{
		fd_set accept_fdset;//fd_set为select的一个数据结构
		struct timeval timeout;
		FD_ZERO(&accept_fdset);//将accept_fdset清零使集合中不含任何fd
		FD_SET(fd,&accept_fdset);//将fd加入accept_fdset集合
		timeout.tv_sec = wait_seconds;//填充超时时间
		timeout.tv_usec = 0;
		do
		{//select会阻塞直到检测到事件或者超时
			ret = select(fd+1,&accept_fdset,NULL,NULL,&timeout);
			//如果select检测到可读事件发送，则此时调用accept不会阻塞			
		} while (ret < 0 && errno == EINTR);
		if (-1 == ret)//失败
		{
			return -1;
		}
		else if (0 == ret)//返回0表示超时	
		{
			errno = ETIMEDOUT;
			return -1;
		}
	}
	if (addr != NULL)
	{//accept
		ret = accept(fd,(struct sockaddr*)addr,&addrlen);
	}
	else
	{
		ret = accept(fd,NULL,NULL);
	}
	if (-1 == ret)
	{
		LCWFTPD_LOG(ERROR,"accept");
	}
	return ret;
}

/**
 *writen - 向fd写入定长的数据
 *@fd:文件描述符
 *@buf:缓冲区
 *@count:要写入的长度
 *成功返回写入的长度，失败返回-1
 */
ssize_t systools::writen(int fd, const void* buf, size_t count)
{
	size_t nleft = count;//剩下的长度
	ssize_t nwritten;//已写入的长度
	char *bufp = (char*)buf;
	while (nleft > 0)//剩下的长度大于0则一直写
	{
		if ((nwritten = write(fd, bufp, nleft)) < 0) //返回写入的长度
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nwritten == 0) 
			continue;		
		bufp += nwritten;//更新已写指针
		nleft -= nwritten;//更新剩下的长度
	}
	return count;
}
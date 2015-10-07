/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：系统工具类实现
  */ 

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
 *writen - 向fd写入buf中定长的数据
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

/**
 *readn - 从fd中读取定长的数据到buf
 *@fd:文件描述符
 *@buf:缓冲区
 *@count:读取长度
 *成功返回读取长度，失败返回-1
 */
ssize_t systools::readn(int fd, void* buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *bufp = (char*)buf;
	while (nleft > 0) 
	{
		if ((nread = read(fd, bufp, nleft)) < 0)//读取nleft长度的数据
		{
     		if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nread == 0) //对方关闭或者已经读到eof
			return count - nleft;//这里返回的就是实际读取到的长度
		
		bufp += nread;//更新读指针
		nleft -= nread;//更新剩下的长度
	}
	
	return count;
}

/**
 *recv_peek - 读取数据到缓冲区buf但是不清除读取缓冲区
 *@socked:socket套接字，recv()只能读写套接字，而不能是一般的文件描述符  
 *@buf:缓冲区buf
 *@len:读取长度
 *返回其实际copy的字节数
 */
ssize_t systools::recv_peek(int sockfd, void* buf, size_t len)
{
	while (1)
	{//MSG_PEEK 查看当前数据,数据将被复制到缓冲区中，但并不从输入队列中删除
		int ret = recv(sockfd, buf, len, MSG_PEEK); 
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

/**
 *readline - 从sockfd读取一行到buf
 *@sockfd:socket套接字
 *@buf:缓冲区
 *@maxline:一行的最大字符数
 *成功返回读取到的长度，失败返回-1
 */
ssize_t systools::readline(int sockfd, void* buf, size_t maxline)
{
//常见的应用层协议都是带有可变长字段的，字段之间的分隔符用换行'\n'的比用'\0'的更常见
// 读到'\n'就返回，加上'\n' 一行最多为maxline个字符 
	int ret;
	int nread;
	char* bufp = (char*)buf;
	int nleft = maxline;
	int count = 0;
	while (1) 
	{   //先用recv_peek看一下现在缓冲区有多少个字符并读取到bufp
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
			return ret; // 返回小于0表示失败
		else if (ret == 0)
			return ret; //返回0表示对方关闭连接了
		
		nread = ret;
		int i;
		for (i = 0; i < nread; i++)//查看是否存在换行符'\n'
		{
			if (bufp[i] == '\n') 
			{//用readn连同换行符一起读取（清空缓冲区）
	 			ret = readn(sockfd, bufp, i+1);
				if (ret != i+1)
					LCWFTPD_LOG(ERROR,"ret != i+1");
				return ret + count;
			}
		}
		if (nread > nleft)
			LCWFTPD_LOG(ERROR,"nread > nleft");
		nleft -= nread;
		//如果不存在'\n'(一行还没结束)，就先读取出来，然后循环读取后面的部分
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			LCWFTPD_LOG(ERROR,"ret != nread");

		bufp += nread;//读取得指针
		count += nread;//已经读取的
	}

	return -1;
}
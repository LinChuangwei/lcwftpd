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
	// strcpy(ip,"192.168.199.170");
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
 *tcp_client - 用于主动模式时向客户端发起连接
 *@port:端口号
 *成功返回监听套接字
 */
int systools::tcp_client(unsigned short port)
{
	int sock;
	if ((sock = socket(PF_INET,SOCK_STREAM,0)) < 0)//建立一个套接字
	{
		LCWFTPD_LOG(ERROR,"tcp_client");
	}
	//对于客户端，一般是不需要绑定端口的
	if (port > 0)
	{
		//设置地址重复利用
		int on = 1;
		if ((setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on))) < 0)
		{
			LCWFTPD_LOG(ERROR,"setsockopt");
		}
		char ip[16] = {0};
		getlocalip(ip);//获取本机IP地址

		struct sockaddr_in localaddr;
		memset(&localaddr,0,sizeof(localaddr));
		localaddr.sin_family = AF_INET;//协议族
		localaddr.sin_port = htons(port);
		localaddr.sin_addr.s_addr = inet_addr(ip);//ip转化为32位的整数
		//绑定
		if (bind(sock,(struct sockaddr*)&localaddr,sizeof(localaddr)) < 0)
		{
			LCWFTPD_LOG(ERROR,"bind");
		}
	}
	return sock;//返回数据套接字
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

/**
 *activate_nonblock - 设置IO为非阻塞模式
 *@fd: 文件描述符
 */ 
void systools::activate_nonblock(int fd) 
{
	int ret;
	int flags =fcntl(fd,F_GETFL);
	if (-1 == flags)
	{
		LCWFTPD_LOG(ERROR,"fcntl");
	}
	flags |= O_NONBLOCK;
	ret = fcntl(fd,F_SETFL,flags);
	if (-1 == ret)
	{
		LCWFTPD_LOG(ERROR,"fcntl");
	}
}

/**
 *deactivate_nonblock - 设置IO为阻塞模式
 *@fd: 文件描述符
 */ 
void systools::deactivate_nonblock(int fd)
{
	int ret;
	int flags = fcntl(fd,F_GETFL);
	if (-1 == flags)
	{
		LCWFTPD_LOG(ERROR,"fcntl");
	}
	flags &= ~O_NONBLOCK;//阻塞
	ret = fcntl(fd,F_SETFL,flags);
	if (-1 == ret)
	{
		LCWFTPD_LOG(ERROR,"fcntl");
	}
}

/**
 *connect_timeout - 带超时的connect
 *@fd: 套接字
 *@addr: 输出参数，返回对方地址
 *@wait_seconds: 等待超时秒数，如果为0表示正常模式
 *返回值：成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int systools::connect_timeout(int fd,struct sockaddr_in* addr,unsigned int wait_seconds)//连接超时函数
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	
	if (wait_seconds > 0)//等待秒数大于0，设置为非阻塞模式
		activate_nonblock(fd);

	ret = connect(fd, (struct sockaddr*)addr, addrlen);
	if (ret < 0 && errno == EINPROGRESS) 
	{//非阻塞，结果是 EINPROGRESS，那么就代表连接还在进行中
	 //后面可以通过poll或者select来判断socket是否可写，如果可以写，说明连接完成了
		fd_set connect_fdset;
		struct timeval timeout;
		FD_ZERO(&connect_fdset);
		FD_SET(fd, &connect_fdset);	
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do {
			//一旦连接建立，套接字就可写
			ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0) 
		{//超时
			errno = ETIMEDOUT;
			return -1;
		}
		else if (ret < 0)
		{//错误
			return -1;
		}
		else if (ret == 1) 
		{
			// ret返回为1，可能有两种情况，一种是连接建立成功，一种是套接字产生错误
			// 此时错误信息不会保存至errno变量中（connect没出错）,因此，需要调用
			// getsockopt来获取 
			int err;
			socklen_t socklen = sizeof(err);
			int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
			if (sockoptret == -1)
				return -1;
			if (err == 0)
				ret = 0;
			else 
			{
				errno = err;
				ret = -1;
			}
		}
	}
	if (wait_seconds > 0)
		deactivate_nonblock(fd);
	return ret;
}

/**
 *statbuf_get_perms - 获取权限
 *@sbuf：文件状态结构体
 *返回权限的数组
 */
const char* systools::statbuf_get_perms(struct stat *sbuf)
{
	//加static的区别？？没加是局部变量，返回时已经销毁，现在是静态变量
	//静态变量生存期是整个源程序
	static char perms[] = "----------";//权限位
	perms[0] = '?';
	mode_t mode = sbuf->st_mode;
	switch(mode & S_IFMT)//S_IFMT 0170000 bit mask for the file type bit fields
	{
		case S_IFREG://regular file普通文件
			perms[0] = '-';
			break;
		case S_IFSOCK://socket 套接字文件
			perms[0] = 's';
			break;
		case S_IFLNK://symbolic link 符号链接文件
			perms[0] = 'l';
			break;
		case S_IFBLK://rblock device块设备文件
			perms[0] = 'b';
			break;
		case S_IFDIR://directory目录文件
			perms[0] = 'd';
			break;
		case S_IFCHR://character device字符设备文件
			perms[0] = 'c';
			break;
		case S_IFIFO://FIFO管道文件
			perms[0] = 'p';
			break;
		default:
			break;
	}
	//用户权限
	if (mode & S_IRUSR)//owner has read permission读权限
	{
		 perms[1] = 'r';
	}
	if (mode & S_IWUSR)//owner has write permission写权限
	{
		 perms[2] = 'w';
	}
	if (mode & S_IXUSR)//owner has execute permissionk可执行权限
	{
		 perms[3] = 'x';
	}
	//组用户权限
	if (mode & S_IRGRP)//group has read permission读权限
	{
		 perms[4] = 'r';
	}
	if (mode & S_IWGRP)//group has write permission写权限
	{
		 perms[5] = 'w';
	}
	if (mode & S_IXGRP)//group has execute permission可执行权限
	{
		 perms[6] = 'x';
	}
	//其他用户权限
	if (mode & S_IROTH)//others have read permission读权限
	{
		 perms[7] = 'r';
	}
	if (mode & S_IWOTH)//others have write permission写权限
	{
		 perms[8] = 'w';
	}
	if (mode & S_IXOTH)//others have execute permission可执行权限
	{
		 perms[9] = 'x';
	}
  	//特殊权限
  	if (mode & S_ISUID)//set-user-ID bit
	{//如果当前具有x的权限，改为s，否则改为S
		 perms[3] = (perms[3] == 'x')?'s':'S';
	}
	if (mode & S_ISGID)//set-group-ID bit
	{//如果当前具有x的权限，改为s，否则改为S
		 perms[6] = (perms[6] == 'x')?'s':'S';
	}
	if (mode & S_ISVTX)//sticky bit
	{//如果当前具有x的权限，改为t，否则改为T
		 perms[9] = (perms[9] == 'x')?'t':'T';
	}
	return perms;
}

/**
 *statbuf_get_data - 获取时间
 *@sbuf：文件状态结构体
 *返回权限的数组
 */
const char* systools::statbuf_get_data(struct stat *sbuf)//获取时间
{
	//时间的格式化
	static char databuf[64] = {0};
	const char* p_data_format = "%b %e %H:%M";//%b月份的简写,%e在两字符域中,十进制表示的每月的第几天
	struct timeval tv;
	gettimeofday(&tv,NULL);
	time_t local_time = tv.tv_sec;
	if (sbuf->st_mtime > local_time || (local_time - sbuf->st_mtime) > 182*24*60*60)//超过半年
	{
		p_data_format = "%b %e %Y";//月份缩写
	}
	//localtime将秒转换成结构体
	struct tm* p_tm = localtime(&local_time);
	strftime(databuf,sizeof(databuf),p_data_format,p_tm);//格式化时间

	return databuf;
}


/**
 *lock_internal - 处理文件的读写锁
 *@fd：文件描述符
 *@lock_type：加锁的类型
 *成功返回0，失败返回-1
 */
int systools::lock_internal(int fd,int lock_type)
{
	//处理锁的过程中还可能被信号中断，还要处理信号中断的问题
  // struct flock {
  //              ...
  //              short l_type;    /* Type of lock: F_RDLCK,
  //                                  F_WRLCK, F_UNLCK */
  //              short l_whence;  /* How to interpret l_start:
  //                                  SEEK_SET, SEEK_CUR, SEEK_END */
  //              off_t l_start;   /* Starting offset for lock */
  //              off_t l_len;     /* Number of bytes to lock */
  //              pid_t l_pid;     /* PID of process blocking our lock
  //                                  (F_GETLK only) */
  //              ...
  //          };
	int ret;
	struct flock the_lock;//定义一个锁的结构体
	memset(&the_lock,0,sizeof(the_lock));
	the_lock.l_type = lock_type;//锁类型 
	the_lock.l_whence = SEEK_SET;//加锁位置：从文件头部开始加锁
	the_lock.l_start = 0;//偏移位置为0
	the_lock.l_len = 0;//加锁的范围字节数，0表示将整个文件加锁
 	do
 	{
 		ret = fcntl(fd,F_SETLKW,&the_lock);//加锁
 	} while (ret < 0 && errno == EINTR);//error==EINTR表示被信号中断
 	//ret==0加锁成功
 	return ret;
}

/**
 *lock_file_read - 文件加读锁
 *@fd:要处理的文件描述符
 */
int systools::lock_file_read(int fd)
{
	return lock_internal(fd,F_RDLCK);
}

/**
 *lock_file_write - 文件写加锁
 *@fd:要处理的文件描述符
 */
int systools::lock_file_write(int fd)
{
	return lock_internal(fd,F_WRLCK);
}

/**
 *unlock_file -  解锁函数
 *@fd:要处理的文件描述符
 */
int systools::unlock_file(int fd)
{
	int ret;
	struct flock the_lock;//定义一个锁的结构体
	memset(&the_lock,0,sizeof(the_lock));
	the_lock.l_type = F_UNLCK;//锁类型 :解锁
	the_lock.l_whence = SEEK_SET;//加锁位置：从文件头部开始加锁
	the_lock.l_start = 0;//偏移位置为0
	the_lock.l_len = 0;//加锁的范围字节数，0表示将整个文件加锁
 	//这里不希望解锁阻塞，不用do while，解锁失败直接返回
 	ret = fcntl(fd,F_SETLKW,&the_lock);//加锁
 	
 	return ret;
}

/**
 *send_fd - 发送文件描述符
 */
void send_fd(int sock_fd,int fd)
{
//发送进程建立一个msghdr结构，其中包含要传递的描述符
	//消息的头部
// struct msghdr {
 //    void         *msg_name;//套接口地址成员
 //    socklen_t    msg_namelen;
 //    struct iovec *msg_iov;//多io缓冲区的地址
 //    size_t       msg_iovlen;/
 //    void         *msg_control;//辅助数据的地址
 //    size_t       msg_controllen;
 //    int          msg_flags;//接收信息标记位
	// };
//控制信息头部
	//辅助数据的数据头部
// struct cmsghdr {
	//附属数据的字节计数，这包含结构头的尺寸。这个值是由CMSG_LEN()宏计算的
//                socklen_t     cmsg_len;     /* data byte count, including hdr */
	//原始的协议级别
//                int           cmsg_level;    /*originating protocol */
//                int           cmsg_type;    /* protocol-specific type */
//            /* followed by
//                unsigned char cmsg_data[]; */
//            };

	// struct iovec  { 
 //    void  * io_base ;   / *  buffer空间的基地址  * / 
 //    size_t iov_len ;   / *  该buffer空间的长度  * / 
	// } ;
	//要发送一个文件描述符的进程必须使用正确的格式化数据来创建一个附属数据缓冲区
	int ret;
	struct msghdr msg_send;//发送
	struct cmsghdr *p_cmsg;
	struct iovec vec;//IO向量
	//CMSG_SPACE这个宏用来计算附属数据以及其头部所需的总空白
	char cmsgbuf[CMSG_SPACE(sizeof(fd))];
	int *p_fds;
	char sendchar = 0;
	//CMSG_FIRSTHDR()宏这个宏用于返回一个指向附属数据缓冲区内的第一个附属对象的struct cmsghdr指针
	//输入值为是指向struct msghdr结构的指针
	p_cmsg = CMSG_FIRSTHDR(&msg_send);
	p_cmsg->cmsg_level = SOL_SOCKET;//原始的协议级别
	p_cmsg->cmsg_type = SCM_RIGHTS;//控制信息类型,SCM_RIGHTS表示附属数据对象是一个文件描述符
	p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));//附属数据的字节计数
	//CMSG_DATA()宏这个宏接受一个指向cmsghdr结构的指针
	p_fds = (int*)CMSG_DATA(p_cmsg);
	*p_fds = fd;

	msg_send.msg_name = NULL;
	msg_send.msg_namelen = 0;
	msg_send.msg_iov = &vec;//io缓冲区的地址
	msg_send.msg_iovlen = 1;
	msg_send.msg_control = cmsgbuf;
	msg_send.msg_controllen = sizeof(cmsgbuf);
	msg_send.msg_flags = 0;

	vec.iov_base = &sendchar;
	vec.iov_len = sizeof(sendchar);

	ret = sendmsg(sock_fd, &msg_send, 0);
	if (ret != 1)
		LCWFTPD_LOG(ERROR,"sendmsg");
}

/**
 *recv_fd - 接收文件描述符
 */
int recv_fd(const int sock_fd)
{
	int ret;
	struct msghdr msg;
	char recvchar;
	struct iovec vec;
	int recv_fd;
	char cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
	struct cmsghdr *p_cmsg;
	int *p_fd;
	vec.iov_base = &recvchar;
	vec.iov_len = sizeof(recvchar);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);
	msg.msg_flags = 0;

	p_fd = (int*)CMSG_DATA(CMSG_FIRSTHDR(&msg));
	*p_fd = -1;  
	ret = recvmsg(sock_fd, &msg, 0);
	if (ret != 1)
		ERR_EXIT("recvmsg");

	p_cmsg = CMSG_FIRSTHDR(&msg);
	if (p_cmsg == NULL)
		ERR_EXIT("no passed fd");


	p_fd = (int*)CMSG_DATA(p_cmsg);
	recv_fd = *p_fd;
	if (recv_fd == -1)
		ERR_EXIT("no passed fd");

	return recv_fd;
}
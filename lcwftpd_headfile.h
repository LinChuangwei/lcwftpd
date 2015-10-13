/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：lcwftpd主要的库头文件及一些声明
  */ 

#ifndef LCWFTPD_HEADFILE_H_
#define LCWFTPD_HEADFILE_H_

#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>//套接字编程
#include <netinet/in.h>//地址
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <map>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
//这个头文件写错了
//#include <sys/capability.h>
//应该是
#include <linux/capability.h>
#include <sys/syscall.h>   // For SYS_xxx definitions  
#include <signal.h>

/**
 *会话结构体
 */
#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND 32
#define MAX_ARG 1024
typedef struct ftp_session
{ 
    //控制连接
    uid_t uid;//用户id
	  int ctrl_fd;//已连接套接字
    char ip[16];//ip
    char cmdline[MAX_COMMAND_LINE];//命令行
    char cmd[MAX_COMMAND];//命令
    char arg[MAX_ARG];//参数
    //数据连接
    struct sockaddr_in* port_addr;//到时要连接的地址
    int pasv_listen_fd;//被动模式套接字
    int data_fd;//数据套接字
    //FTP协议状态
    int is_ascii;//是否是ascii码状态

}session_t;




//'\'后面不要加注释
/**
 *LCWFTPD_LOG - 日志宏
 *输出日期，时间，日志级别，源码文件，行号，信息
 */
 //定义一个日志宏
#define DEBUG 0
#define INFO  1
#define WARN  2
#define ERROR 3
#define CRIT  4 

static const char* LOG_STR[] = { 
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CRIT"
};

#define LCWFTPD_LOG(level, format, ...) do{ \
    if (level >= 0) {\
        time_t now = time(NULL);                      \
        char msg[1024];                        \
        char buf[32];                                   \
        sprintf(msg, format, ##__VA_ARGS__);             \
        strftime(buf, sizeof(buf), "%Y%m%d %H:%M:%S", localtime(&now)); \
        fprintf(stdout, "[%s] [%s] [file:%s] [line:%d] %s\n",buf,LOG_STR[level],__FILE__,__LINE__, msg); \
        fflush (stdout); \
    }\
     if (level >= ERROR) {\
        exit(-1); \
    } \
} while(0)


#endif
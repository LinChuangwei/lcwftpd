#ifndef LCWFTPD_HEADFILE_H_
#define LCWFTPD_HEADFILE_H_
//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief： lcwftpd主要的库头文件

#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

//定义一个日志宏
#define DEBUG 0
#define INFO  1
#define WARN  2
#define ERROR 3
#define CRIT  4 

static const char * LOG_STR[] = { 
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CRIT"
};

//可变参数 
//输出日期，时间，日志级别，源码文件，行号，信息
//'\'后面不要加注释
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
//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief：main函数
#include "lcwftpd_headfile.h"
#include "lcwftpd_loop.h"
int main(int argc, char const *argv[])
{
	LCWFTPD_LOG(DEBUG,"lcwftpd by LinChuangwei.");//打印
	lcwftpd_loop ftploop;
	ftploop.lcwftpd_init();//ftp服务器初始化
	ftploop.lcwftpd_run();//开启ftp的循环
	return 0;
}
//start from the very beginning,and to create greatness
//@author: Chuangwei Lin
//@E-mail：979951191@qq.com
//@brief：main函数
#include "lcwftpd_headfile.h"

int main(int argc, char const *argv[])
{
	if (getuid() != 0)//如果不是root用户启动
	{
		LCWFTPD_LOG(ERROR,"lcwftpd must be started as root");
	}
	LCWFTPD_LOG(DEBUG,"Welcome use lcwftpd");
	return 0;
}
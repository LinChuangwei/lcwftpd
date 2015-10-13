/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：nobody进程处理相关
  */ 
#ifndef NOBODY_H_
#define NOBODY_H_
#include "lcwftpd_headfile.h"




class nobody
{
public:
	//处理nobod进程
	void handle_nobody(session_t* sess);
	//s设置特权，使特权最小化
	void minimize_privilege();
	//用系统调用实现capset
	int capset(cap_user_header_t hdrp, const cap_user_data_t datap);
	
};

#endif
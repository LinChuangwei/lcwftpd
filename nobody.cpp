/**
  *start from the very beginning,and to create greatness
  *@author: LinChuangwei 
  *@E-mail：979951191@qq.com
  *@brief：nobody进程处理相关的实现文件
  */ 
#include "nobody.h"

/**
 *handle_nobody - nobody进程的处理函数
 *@sess：会话结构体
 */
void nobody::handle_nobody(session_t* sess)
{
	LCWFTPD_LOG(DEBUG,"Hello handle_nobody");
	//nobody进程作为辅助进程，既需要一定的特权，但也不一定需要root权限
	//用该函数实现nobody进程特权最小化
	minimize_privilege();
	while(1);
}

//用系统调用实现capset
int nobody::capset(cap_user_header_t hdrp, const cap_user_data_t datap)
{
	return syscall(__NR_capset,hdrp,datap);//调用内核当中原始的接口
	//要包括#include <sys/syscall.h>   // For SYS_xxx definitions  
}
//s设置特权，使特权最小化
void nobody::minimize_privilege()
{
	//将当前进程设置为nobody进程
	struct passwd* pw = getpwnam("nobody");
	if (pw == NULL)
	{
		return;
	}
	LCWFTPD_LOG(DEBUG,"pw->pw_gid:%d",pw->pw_gid);
	LCWFTPD_LOG(DEBUG,"pw->pw_uid:%d",pw->pw_uid);
	//在没有更改之前，主id和用户id都是等于0的，因为我们是以root启动的
	if (setegid(pw->pw_gid) < 0)//主id
	{
	 	LCWFTPD_LOG(ERROR,"setegid");
	}
	if (seteuid(pw->pw_uid) < 0)//用户id
	{
		LCWFTPD_LOG(ERROR,"seteuid");
	}
   //让nobody具有绑定端口的权限
	struct __user_cap_header_struct cap_header;
	struct __user_cap_data_struct cap_data;
	memset(&cap_header,0,sizeof(cap_header));
	memset(&cap_data,0,sizeof(cap_data));
	cap_header.version = _LINUX_CAPABILITY_VERSION_1;//64位的系统用这个_LINUX_CAPABILITY_VERSION_2,但是好像失败了
	cap_header.pid = 0;
	__u32 cap_mask = 0;
	//赋值特权
	cap_mask |= (1 << CAP_NET_BIND_SERVICE);
	cap_data.effective = cap_data.permitted = cap_mask;//赋予特权
	cap_data.inheritable = 0;//继承的权限，不允许
	capset(&cap_header,&cap_data);//capset是一个原始的内核接口

   //		  typedef struct __user_cap_header_struct {
   //            __u32 version;
   //            int pid;
   //         } *cap_user_header_t;

   //         typedef struct __user_cap_data_struct {
   //            __u32 effective;//应该具有的权限
   //            __u32 permitted;//
   //            __u32 inheritable;//继承到新进程
   //         } *cap_user_data_t;

}
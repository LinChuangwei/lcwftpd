/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：字符串处理类实现
 */

#include "ftpstr.h"
void ftpstr::fangjinggao()
{
	LCWFTPD_LOG(DEBUG,"防警告");
}


/**
 *str_trim_crlf - 去除\r\n
 *@str:要处理的字符串
 */
void ftpstr::str_trim_crlf(char* str)
{
	char* p = &str[strlen(str) -1];//p指向str最后一个字符
	while(*p == '\r' || *p =='\n')
	{
		*p-- = '\0';
	}
}

/**
 *str_split - 分割字符串
 *@str:要处理的字符串
 *@left:存放分割后左边的字符串
 *@right:存放分割后右边的字符串
 *@c:分割依据
 */
void ftpstr::str_split(const char* str,char* left,char* right,char c)
{
	//查找第一个c字符所在位置
	char* p = strchr((char*)str,c);//strstr是查找字符串的，strchr是查找字符的
	if (p == NULL)//没有找到
	{
		strcpy(left,str);
	}
	else
	{
		*p = '\0';
		strcpy(left,str);//复制左边
		strcpy(right,p+1);//复制右边
	}
}

/**
 *str_upper - 字母转换成大写
 *@str：要处理的字符串
 */
void ftpstr::str_upper(char* str)
{
	while(*str)
	{
		*str = toupper(*str);//转化后通过返回值返回
		str++;
	}
}

/**
 *delspace - 去除字符串首尾的空格
 *@str:要处理的字符串
 *返回处理好的字符串
 */
char* ftpstr::delspace(char* str)
{
	char* end,*sp,*ep;//结尾指针，两个游标
	size_t len;//用于保存长度长度
	sp = str;//sp指针首先指向行首
	end = ep = str + strlen(str) -1;//end指针指向结尾
	//从行首开始去空格
	while(sp <= end && isspace(*sp))//是空格返回非零
		sp++;//移动sp指针直到指向的不是空格
	//从行尾去除空格
	while(ep >= sp && isspace(*ep))
		ep--;
	len = (ep < sp)? 0 :(ep - sp)+1;//计算长度+1,指向换行符
	sp[len] = '\0';//换行符置为'\0'
	return sp;
}
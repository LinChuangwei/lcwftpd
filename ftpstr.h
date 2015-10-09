/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：字符串处理类头文件
 */

#include "lcwftpd_headfile.h"

 class ftpstr
 {
 public:
 	 //去除\r\n
 	 void str_trim_crlf(char* str);
 	 //分割字符串
 	 void str_split(const char* str,char* left,char* right,char c);
 	 //字母转换成大写
 	 void str_upper(char* str);
 };
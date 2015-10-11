/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：配置文件解析类头文件
 */

 #ifndef PARSECONFIG_H_
 #define PARSECONFIG_H_



#include "lcwftpd_headfile.h"
#include "ftpstr.h"
//配置文件
#define CONF_FILE "lcwftpd.conf"
//每一行的最大长度
#define MAX_CONF_LEN 1024

//配置文件的解析类定义成成单例类
class parseconfig
{
public:
	//返回该单例对象
	static parseconfig& instance();
	//加载配置文件将参数存入
	int loadfile();
	//一些访问私有成员的接口，有点啰嗦，不妨实现一番
	bool get_pasv_active();
	bool get_port_active();
	unsigned int get_listen_port();
	unsigned int get_max_clients();
	unsigned int get_max_per_ip();
	unsigned int get_accept_timeout();
	unsigned int get_connect_timeout();
	unsigned int get_idle_session_timeout();
	unsigned int get_data_connection_timeout();
	unsigned int get_local_umask();
	unsigned int get_upload_max_rate();
	unsigned int get_download_max_rate();
	char* get_listen_address();

private:
	//定义一个字符串处理类
	ftpstr lcwftpstr;
	//构造函数
	parseconfig();
	//释放内存
	static void destroy();
	//一个pasv和port配置的处理函数
	bool handle_pasv_port(char* key,char* value,int linenumber);
	//一个配置unsigned int类型配置项的函数
	void configuint(char* key,char* value,int linenumber);

	//定义自身单例类
	static parseconfig* __self;
	bool pasv_active;//被动模式
	bool port_active;//主动模式
	unsigned int listen_port;//监听端口 
	unsigned int max_clients;//最大连接数
	unsigned int max_per_ip;//每ip的最大连接数
	unsigned int accept_timeout;//接收超时时间
	unsigned int connect_timeout;//连接超时时间
	unsigned int idle_session_timeout;//空闲断开连接时间
	unsigned int data_connection_timeout;// 数据连接断开时间
	unsigned int local_umask;//掩码
	unsigned int upload_max_rate;//上传最大速率
	unsigned int download_max_rate;//下载最大速率
	char* listen_address;//监听的地址
};



 #endif


